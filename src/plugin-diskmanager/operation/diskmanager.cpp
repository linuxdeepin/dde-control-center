// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "diskmanager.h"

#include "dccfactory.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QTextStream>
#include <QDebug>
#include <QLoggingCategory>

#include <sys/statvfs.h>

// 日志分类（过滤日志时可以用: -l diskmanager,debug）
Q_LOGGING_CATEGORY(dccDiskLog, "org.deepin.dde.control-center.diskmanager")

/**
 * @brief 辅助函数: 将字节数转为人类可读格式
 * @param bytes 字节数
 * @return 格式化字符串，如 "256.1 GB"
 */
static QString formatBytes(qint64 bytes)
{
    if (bytes <= 0) return "0 B";
    static const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    return QString::number(size, 'f', 1) + " " + QString(units[unitIndex]);
}

/**
 * @brief 判断磁盘类型: 根据 /sys/block/<name>/queue/rotational 判断
 * @param deviceName 设备名（如 sda）
 * @return "SSD" / "HDD" / "Unknown"
 */
static QString detectDiskType(const QString &deviceName)
{
    QFile rotFile(QStringLiteral("/sys/block/%1/queue/rotational").arg(deviceName));
    if (rotFile.open(QIODevice::ReadOnly)) {
        QString val = QString::fromUtf8(rotFile.readAll()).trimmed();
        rotFile.close();
        if (val == "0") return "SSD";
        if (val == "1") return "HDD";
    }
    return "Unknown";
}

/**
 * @brief 读取 /proc/partitions 获取磁盘列表
 * @return 磁盘信息列表
 *
 * /proc/partitions 格式:
 * major minor  #blocks  name
 *   8    0   250059096 sda
 *   8    1   249990144 sda1
 * 只取主设备（如 sda, nvme0n1），跳过分区（sda1, sda2...）
 */
static QVariantList readDiskInfoFromProc()
{
    QVariantList result;
    QFile file("/proc/partitions");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(dccDiskLog) << "Cannot open /proc/partitions";
        return result;
    }

    QSet<QString> seenDevices;
    QTextStream in(&file);
    in.readLine(); // 跳过标题行

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 4) continue;

        QString name = parts[3];

        // 判断是否是分区（不是主设备）
        // 规则:
        //   - sda, nvme0n1, mmcblk0 等 → 主设备，保留
        //   - sda1, nvme0n1p1, mmcblk0p1 → 分区，跳过
        // 主设备一定在 /sys/block/ 下有对应目录
        bool isPartition = false;
        {
            // 检查 /sys/block/<name> 是否存在 —— 主设备一定存在
            QDir sysBlockDir(QStringLiteral("/sys/block/%1").arg(name));
            if (!sysBlockDir.exists()) {
                // 不存在于 /sys/block/ 中，可能是分区，也可能是虚拟设备
                isPartition = true;
            }
        }

        if (isPartition) continue;
        if (seenDevices.contains(name)) continue;
        seenDevices.insert(name);

        // 读取设备大小（#blocks 是 1024 字节块数）
        qint64 blocks = parts[2].toLongLong();
        qint64 sizeBytes = blocks * 1024;

        QVariantMap disk;
        disk["name"] = name;
        disk["size"] = formatBytes(sizeBytes);
        disk["sizeBytes"] = sizeBytes;

        // 读取 /sys/block/<name>/device/model 获取型号
        QFile modelFile(QStringLiteral("/sys/block/%1/device/model").arg(name));
        if (modelFile.open(QIODevice::ReadOnly)) {
            disk["model"] = QString::fromUtf8(modelFile.readAll()).trimmed();
            modelFile.close();
        } else {
            disk["model"] = "";
        }

        disk["type"] = detectDiskType(name);

        result.append(disk);
    }

    file.close();
    return result;
}

/**
 * @brief 读取 /proc/mounts + QStorageInfo 获取挂载点 + 使用率
 * @return 挂载点信息列表
 *
 * /proc/mounts 格式:
 * /dev/sda1 / ext4 rw,relatime 0 0
 *
 * 通过 QStorageInfo 获取使用率，过滤掉无关的虚拟文件系统
 */
static QVariantList readMountInfo()
{
    QVariantList result;
    QFile file("/proc/mounts");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(dccDiskLog) << "Cannot open /proc/mounts";
        return result;
    }

    // 需要忽略的虚拟/伪文件系统
    static const QStringList skipPrefixes = {
        "sysfs", "proc", "devtmpfs", "securityfs", "tmpfs",
        "devpts", "cgroup", "pstore", "efivarfs", "bpf",
        "mqueue", "hugetlbfs", "debugfs", "tracefs",
        "ramfs", "fusectl", "overlay", "autofs",
        "binfmt_misc", "sunrpc", "configfs", "squashfs"
    };

    auto shouldSkip = [](const QString &fsType) -> bool {
        for (const auto &p : skipPrefixes) {
            if (fsType.startsWith(p)) return true;
        }
        return false;
    };

    QTextStream in(&file);
    int debugCount = 0, debugSkip = 0, debugNotDev = 0, debugStatvfsFail = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 3) continue;

        QString device = parts[0];
        QString mountPoint = parts[1];
        QString fsType = parts[2];
        QString mountOptions = parts.size() > 3 ? parts[3] : "";

        // 跳过虚拟文件系统
        if (shouldSkip(fsType)) { debugSkip++; continue; }
        // 只处理 /dev/ 开头的真实磁盘挂载
        if (!device.startsWith("/dev/")) { debugNotDev++; continue; }

        debugCount++;
        // 使用 statvfs() 获取用量信息
        struct statvfs vfs;
        if (statvfs(mountPoint.toUtf8().constData(), &vfs) != 0) {
            qCDebug(dccDiskLog) << "statvfs failed for" << device << mountPoint << strerror(errno);
            debugStatvfsFail++;
            continue;
        }

        qint64 blockSize = static_cast<qint64>(vfs.f_frsize);
        qint64 total = static_cast<qint64>(vfs.f_blocks) * blockSize;
        qint64 available = static_cast<qint64>(vfs.f_bavail) * blockSize;
        qint64 used = total - available;
        // 注意: f_blocks 包含保留块，所以 total 可能比 df 显示的略大
        // 使用 f_blocks - f_bfree 来计算已用空间更准确
        qint64 free = static_cast<qint64>(vfs.f_bfree) * blockSize;
        used = total - free;
        int usagePercent = (total > 0) ? static_cast<int>(used * 100 / total) : 0;

        QVariantMap mountInfo;
        mountInfo["device"] = device;
        mountInfo["mountPoint"] = mountPoint;
        mountInfo["filesystem"] = fsType;
        mountInfo["total"] = formatBytes(total);
        mountInfo["totalBytes"] = total;
        mountInfo["used"] = formatBytes(used);
        mountInfo["usedBytes"] = used;
        mountInfo["available"] = formatBytes(available);
        mountInfo["availableBytes"] = available;
        mountInfo["usagePercent"] = usagePercent;
        mountInfo["mountOptions"] = mountOptions;

        result.append(mountInfo);
    }

    file.close();
    return result;
}

// ============================================================
// DiskManager 实现
// ============================================================

DiskManager::DiskManager(QObject *parent)
    : QObject(parent)
{
    qCDebug(dccDiskLog) << "DiskManager constructed";
    // 构造时立即刷新一次数据
    refreshInternal();
}

QVariantList DiskManager::diskInfoList() const
{
    return m_diskInfoList;
}

QVariantList DiskManager::mountInfoList() const
{
    return m_mountInfoList;
}

void DiskManager::refresh()
{
    qCDebug(dccDiskLog) << "Refreshing disk info...";
    refreshInternal();
    // 发射信号通知 QML 更新 UI
    Q_EMIT diskInfoChanged();
}

/**
 * @brief 判断设备名是否属于某个磁盘
 * @param deviceName 设备名，如 "sda1"、"nvme0n1p1"
 * @param diskName 磁盘名，如 "sda"、"nvme0n1"
 * @return true 如果设备是该磁盘的分区
 */
static bool isPartitionOfDisk(const QString &deviceName, const QString &diskName)
{
    if (!deviceName.startsWith(diskName)) return false;
    // 确保 deviceName 比 diskName 长，且后续字符是数字或 'p'
    if (deviceName.length() <= diskName.length()) return false;
    QString suffix = deviceName.mid(diskName.length());
    if (suffix.startsWith('p')) suffix = suffix.mid(1);
    for (const QChar &c : suffix) {
        if (!c.isDigit()) return false;
    }
    return true;
}

void DiskManager::refreshInternal()
{
    m_diskInfoList = readDiskInfoFromProc();
    m_mountInfoList = readMountInfo();

    // 为每个磁盘聚合分区用量数据
    // 遍历每个磁盘，从 mountInfoList 中找出属于该磁盘的分区并累加
    for (int i = 0; i < m_diskInfoList.size(); ++i) {
        QVariantMap disk = m_diskInfoList[i].toMap();
        QString diskName = disk["name"].toString();

        qint64 totalBytes = 0;
        qint64 usedBytes = 0;
        qint64 availableBytes = 0;
        int partitionCount = 0;

        for (const auto &mountVar : m_mountInfoList) {
            QVariantMap mount = mountVar.toMap();
            QString device = mount["device"].toString();
            // 提取设备名（去掉 /dev/ 前缀）
            QString devName = device;
            if (devName.startsWith("/dev/")) devName = devName.mid(5);

            if (isPartitionOfDisk(devName, diskName)) {
                totalBytes += mount["totalBytes"].toLongLong();
                usedBytes += mount["usedBytes"].toLongLong();
                availableBytes += mount["availableBytes"].toLongLong();
                partitionCount++;
            }
        }

        // 只挂载了部分分区时，用已挂载的分区数据计算使用率
        int usagePercent = 0;
        if (totalBytes > 0) {
            usagePercent = static_cast<int>(usedBytes * 100 / totalBytes);
        }

        disk["totalBytes"] = totalBytes;
        disk["usedBytes"] = usedBytes;
        disk["availableBytes"] = availableBytes;
        disk["usagePercent"] = usagePercent;
        disk["partitionCount"] = partitionCount;
        disk["totalFormatted"] = totalBytes > 0 ? formatBytes(totalBytes) : "N/A";
        disk["usedFormatted"] = usedBytes > 0 ? formatBytes(usedBytes) : "N/A";
        disk["availableFormatted"] = availableBytes > 0 ? formatBytes(availableBytes) : "N/A";
        disk["hasUsageData"] = partitionCount > 0;

        m_diskInfoList[i] = disk;
    }

    qCDebug(dccDiskLog) << "Found" << m_diskInfoList.size() << "disks,"
                         << m_mountInfoList.size() << "mount points";
}

// ============================================================
// ★ 插件注册 —— 整个流程中至关重要的一步 ★
//
// DCC_FACTORY_CLASS 宏会生成一个 DiskManagerDccFactory 类：
//   - 继承自 DccFactory（在 dccfactory.h 中定义）
//   - 包含 Q_PLUGIN_METADATA(IID "org.deepin.dde.dcc-factory/v1.0")
//   - 实现 create() 方法，返回 new DiskManager(parent)
//
// 框架在插件加载流程的 Data 阶段调用 DccFactory::create()，
// 获取 DiskManager 实例，并导出为 dccData 供 QML 使用。
// ============================================================
DCC_FACTORY_CLASS(DiskManager)// DCC_FACTORY_CLASS 在 dccfactory.h 中定义，自动生成 DiskManagerDccFactory 类并注册 Qt 插件
#include "diskmanager.moc"