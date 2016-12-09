#include "systeminfomodel.h"

namespace dcc{
namespace systeminfo{

SystemInfoModel::SystemInfoModel(QObject *parent)
    : QObject(parent)
{

}

void SystemInfoModel::setEntryLists(const QStringList &list)
{
    m_entryLists = list;
}

void SystemInfoModel::setDefaultEntry(const QString &entry)
{
    m_defaultEntry = entry;
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
