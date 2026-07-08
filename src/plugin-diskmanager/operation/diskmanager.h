// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

/**
 * @brief DiskManager - 磁盘管理插件的 C++ 后端
 *
 * 【插件体系中的作用】
 * 每个插件都需要一个 C++ 后端类，通过 DCC_FACTORY_CLASS 宏注册到框架。
 * 框架加载插件时调用 DccFactory::create() 创建该对象，作为插件的"数据层"。
 *
 * 【与 QML 的交互方式】
 * 在 QML 中，该对象可通过 dccData 全局变量访问（框架自动注入）。
 * 例如: dccData.diskInfoList() 返回磁盘信息列表。
 *
 * 【插件加载流程中的位置】
 * 在 DccPluginLoader 状态机的 Data 阶段被创建:
 *   MetaData → Module → ☆Data☆ → MainObj → PluginEnd
 * Data 阶段在 QThreadPool 工作线程中异步执行，所以构造函数应避免耗时操作。
 */
class DiskManager : public QObject
{
    Q_OBJECT
    // Q_PROPERTY 暴露给 QML 的属性，QML 中通过 dccData.<属性名> 访问
    Q_PROPERTY(QVariantList diskInfoList READ diskInfoList NOTIFY diskInfoChanged FINAL)
    Q_PROPERTY(QVariantList mountInfoList READ mountInfoList NOTIFY diskInfoChanged FINAL)

public:
    /**
     * @brief 构造函数
     * @param parent Qt 父对象
     *
     * 注意：插件加载时 Data 阶段在线程池中异步执行，
     * 所以构造函数中不要操作 GUI 相关的内容。
     * 但读取 /proc 文件系统是安全的。
     */
    explicit DiskManager(QObject *parent = nullptr);

    /**
     * @brief 获取磁盘列表
     * @return QVariantList, 每个元素是包含磁盘信息的 QVariantMap
     *
     * 返回数据结构示例:
     * [
     *   {
     *     "name": "sda",
     *     "size": "256.1 GB",
     *     "model": "Samsung SSD 860...",
     *     "type": "SSD"
     *   },
     *   ...
     * ]
     */
    QVariantList diskInfoList() const;

    /**
     * @brief 获取挂载点信息（分区 + 使用率）
     * @return QVariantList, 每个元素是包含挂载点详情的 QVariantMap
     *
     * 返回数据结构示例:
     * [
     *   {
     *     "device": "/dev/sda1",
     *     "mountPoint": "/",
     *     "filesystem": "ext4",
     *     "total": "240.0 GB",
     *     "used": "45.2 GB",
     *     "available": "182.3 GB",
     *     "usagePercent": 19,
     *     "mountOptions": "rw,relatime"
     *   },
     *   ...
     * ]
     */
    QVariantList mountInfoList() const;

public Q_SLOTS:
    /**
     * @brief 刷新磁盘信息
     *
     * QML 中调用: dccData.refresh()
     * 触发 diskInfoChanged 信号，QML 端自动更新 UI
     */
    void refresh();

Q_SIGNALS:
    /**
     * @brief 磁盘信息变化信号
     * 调用 refresh() 或底层数据变化时发射
     */
    void diskInfoChanged();

private:
    /**
     * @brief 内部刷新数据（线程安全）
     * 读取 /proc/partitions、/proc/mounts、statvfs 等
     */
    void refreshInternal();

    /** 缓存的磁盘列表 */
    QVariantList m_diskInfoList;
    /** 缓存的挂载点列表 */
    QVariantList m_mountInfoList;
};

#endif // DISKMANAGER_H