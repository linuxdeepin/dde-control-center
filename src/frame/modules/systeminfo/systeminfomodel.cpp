// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "systeminfomodel.h"

#include "math.h"

namespace dcc{
namespace systeminfo{

static QString formatCap(qulonglong cap, const int size = 1024, quint8 precision = 1)
{
    static QStringList type = {" B", " KB", " MB", " GB", " TB"};
    qulonglong lc = cap;
    double dc = cap;
    double ds = size;

    for(int p = 0; p < type.count(); ++p) {
        if (cap < pow(size, p + 1) || p == type.count() - 1) {
            if (!precision) {
#ifdef __sw_64__
                return QString::number(ceil(lc / pow(size, p))) + type[p];
#else
                return QString::number(round(lc / pow(size, p))) + type[p];
#endif
            }

            return QString::number(dc / pow(ds, p), 'f', precision) + type[p];
        }
    }

    return "";
}

SystemInfoModel::SystemInfoModel(QObject *parent)
    : QObject(parent)
    , m_bootDelay(false)
    , m_themeEnabled(false)
    , m_updating(false)
    , m_type(64)
    , m_licenseState(ActiveState::Unauthorized)
{

}

void SystemInfoModel::setEntryLists(const QStringList &list)
{
    if (list != m_entryLists) {
        m_entryLists = list;
        Q_EMIT entryListsChanged(list);
    }
}

void SystemInfoModel::setThemeEnabled(const bool enabled)
{
    if (m_themeEnabled != enabled) {
        m_themeEnabled = enabled;
        Q_EMIT themeEnabledChanged(m_themeEnabled);
    }
}

void SystemInfoModel::setDefaultEntry(const QString &entry)
{
    if (m_defaultEntry != entry) {
        m_defaultEntry = entry;
        Q_EMIT defaultEntryChanged(entry);
    }
}

void SystemInfoModel::setUpdating(bool updating)
{
    if (updating != m_updating) {
        m_updating = updating;
        Q_EMIT updatingChanged(updating);
    }
}

bool SystemInfoModel::bootDelay() const
{
    return m_bootDelay;
}

void SystemInfoModel::setBootDelay(bool bootDelay)
{
    if (m_bootDelay != bootDelay) {
        m_bootDelay = bootDelay;
        Q_EMIT bootDelayChanged(bootDelay);
    }
}


QPixmap SystemInfoModel::background() const
{
    return m_background;
}

void SystemInfoModel::setBackground(const QPixmap &background)
{
    m_background = background;

    Q_EMIT backgroundChanged(background);
}


void SystemInfoModel::setDistroID(const QString &distroid)
{
    if(m_distroid == distroid)
        return;

    m_distroid = distroid;
    Q_EMIT distroidChanged(m_distroid);
}

void SystemInfoModel::setDistroVer(const QString &distrover)
{
    if(m_distrover == distrover)
        return;

    m_distrover = distrover;
    Q_EMIT distroverChanged(m_distrover);
}

void SystemInfoModel::setProductName(const QString& name)
{
    if(m_productName == name)
        return;

    m_productName = name;
    Q_EMIT productNameChanged(m_productName);
}
void SystemInfoModel::setVersionNumber(const QString& number)
{
    if(m_versionNumber == number)
        return;

    m_versionNumber = number;
    Q_EMIT versionNumberChanged(m_versionNumber);
}

void SystemInfoModel::setVersion(const QString &version)
{
    if(version == "" || m_version == version)
        return;

    m_version = version;
    Q_EMIT versionChanged(m_version);
}

void SystemInfoModel::setType(qlonglong type)
{
    if(m_type == QString("%1").arg(type))
        return ;

    m_type = QString("%1").arg(type);
    typeChanged(m_type);
}

void SystemInfoModel::setProcessor(const QString &processor)
{
    if(m_processor == processor)
        return;

    m_processor = processor;
    processorChanged(processor);
}

void SystemInfoModel::setHostName(const QString &hostName)
{
    if(m_hostName == hostName)
        return;

    m_hostName = hostName;
    hostNameChanged(hostName);
}

void SystemInfoModel::setMemory(qulonglong totalMemory, qulonglong installedMemory)
{
    QString mem_device_size = formatCap(installedMemory, 1024, 0);
    QString mem = formatCap(totalMemory);
    if(m_memory == mem)
        return ;

    m_memory = mem;
    m_memory = QString("%1 (%2 %3)").arg(mem_device_size, mem, tr("available"));
    memoryChanged(m_memory);
}

void SystemInfoModel::setDisk(qulonglong disk)
{

    QString d = formatCap(disk);
    if(m_disk == d)
        return ;

    m_disk = d;
    diskChanged(m_disk);
}

void SystemInfoModel::setKernel(const QString &kernel)
{
    if (m_kernel == kernel)
        return;

    m_kernel = kernel;
    kernelChanged(kernel);
}

void SystemInfoModel::setLicenseState(ActiveState state)
{
    if (m_licenseState != state) {
        m_licenseState = state;
        Q_EMIT licenseStateChanged(state);
    }
}

}
}
