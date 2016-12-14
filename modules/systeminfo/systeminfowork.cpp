#include "systeminfowork.h"
#include "systeminfomodel.h"

namespace dcc{
namespace systeminfo{

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    :QObject(parent),
      m_model(model)
{
    m_systemInfoInter = new SystemInfoInter("com.deepin.daemon.SystemInfo",
                                            "/com/deepin/daemon/SystemInfo",
                                            QDBusConnection::sessionBus(),this);

    m_dbusGrub = new GrubDbus("com.deepin.daemon.Grub2",
                              "/com/deepin/daemon/Grub2",
                              QDBusConnection::sessionBus(),
                              this);

    connect(m_systemInfoInter, SIGNAL(VersionChanged(QString)), this, SIGNAL(VersionChanged(QString)));
    connect(m_systemInfoInter, SIGNAL(SystemTypeChanged(qlonglong)), this, SIGNAL(SystemTypeChanged(qlonglong)));
    connect(m_systemInfoInter, SIGNAL(ProcessorChanged(QString)), this, SIGNAL(ProcessorChanged(QString)));
    connect(m_systemInfoInter, SIGNAL(MemoryCapChanged(qulonglong)), this, SIGNAL(MemoryCapChanged(qulonglong)));
    connect(m_systemInfoInter, SIGNAL(DiskCapChanged(qulonglong)), this, SIGNAL(DiskCapChanged(qulonglong)));
    connect(m_dbusGrub,SIGNAL(DefaultEntryChanged(QString)), m_model, SLOT(setDefaultEntry(QString)));
}

QString SystemInfoWork::version() const
{
    return m_systemInfoInter->version();
}

qlonglong SystemInfoWork::type() const
{
    return m_systemInfoInter->systemType();
}

QString SystemInfoWork::processor() const
{
    return m_systemInfoInter->processor();
}

qlonglong SystemInfoWork::memory() const
{
    return m_systemInfoInter->memoryCap();
}

qlonglong SystemInfoWork::disk() const
{
    return m_systemInfoInter->diskCap();
}

QStringList SystemInfoWork::entryLists() const
{
    return m_dbusGrub->GetSimpleEntryTitles();
}

QString SystemInfoWork::defaultEntry() const
{
    return m_dbusGrub->defaultEntry();
}

void SystemInfoWork::setBootDelay(bool value)
{

    if(value)
    {
        QStringList list = m_dbusGrub->GetSimpleEntryTitles();
        if(list.count() == 1)
        {
            m_dbusGrub->setTimeout(1);
        }
        else
        {
            m_dbusGrub->setTimeout(5);
        }
    }
    else
    {
        m_dbusGrub->setTimeout(1);
    }
}

void SystemInfoWork::setEnableTheme(bool value)
{
    m_dbusGrub->setEnableTheme(value);
}

void SystemInfoWork::setDefaultEntry(const QString &entry)
{
    m_dbusGrub->setDefaultEntry(entry);
}

}
}
