#include "systeminfomodel.h"

namespace dcc{
namespace systeminfo{

static QString formatCap(qulonglong cap, const int size = 1024)
{
    static QString type[] = {"B", "KB", "MB", "GB", "TB"};

    if (cap < qulonglong(size)) {
        return QString::number(cap) + type[0];
    }
    if (cap < qulonglong(size) * size) {
        return QString::number(double(cap) / size, 'f', 2) + type[1];
    }
    if (cap < qulonglong(size) * size * size) {
        return QString::number(double(cap) / size / size, 'f', 2) + type[2];
    }
    if (cap < qulonglong(size) * size * size * size) {
        return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];
    }
    return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
}

SystemInfoModel::SystemInfoModel(QObject *parent)
    : QObject(parent)
{

}

#ifndef DCC_DISABLE_GRUB
void SystemInfoModel::setEntryLists(const QStringList &list)
{
    if (list != m_entryLists) {
        m_entryLists = list;
        emit entryListsChanged(list);
    }
}

void SystemInfoModel::setThemeEnabled(const bool enabled)
{
    if (m_themeEnabled != enabled) {
        m_themeEnabled = enabled;
        emit themeEnabledChanged(m_themeEnabled);
    }
}

void SystemInfoModel::setDefaultEntry(const QString &entry)
{
    if (m_defaultEntry != entry) {
        m_defaultEntry = entry;
        emit defaultEntryChanged(entry);
    }
}

void SystemInfoModel::setUpdating(bool updating)
{
    if (updating != m_updating) {
        m_updating = updating;
        emit updatingChanged(updating);
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
        emit bootDelayChanged(bootDelay);
    }
}
#endif

void SystemInfoModel::setDistroID(const QString &distroid)
{
    if(m_distroid == distroid)
        return;

    m_distroid = distroid;
    emit distroidChanged(m_distroid);
}

void SystemInfoModel::setDistroVer(const QString &distrover)
{
    if(m_distrover == distrover)
        return;

    m_distrover = distrover;
    emit distroverChanged(m_distrover);
}

void SystemInfoModel::setVersion(const QString &version)
{
    if(m_version == version)
        return;

    m_version = version;
    emit versionChanged(m_version);
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
    processorChanged(m_processor);
}

void SystemInfoModel::setMemory(qulonglong memory)
{
    QString mem = formatCap(memory);
    if(m_memory == mem)
        return ;

    m_memory = mem;
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

}
}
