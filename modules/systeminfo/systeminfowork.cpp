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

    m_systemInfoInter->setSync(false);
    m_dbusGrub->setSync(false);

    connect(m_dbusGrub, &GrubDbus::DefaultEntryChanged, m_model, &SystemInfoModel::setDefaultEntry);
    connect(m_dbusGrub, &GrubDbus::EnableThemeChanged, m_model, &SystemInfoModel::setThemeEnabled);
    connect(m_dbusGrub, &GrubDbus::TimeoutChanged, this, [this] (const int &value) {
        m_model->setBootDelay(value > 1);
    });

    connect(m_systemInfoInter, &__SystemInfo::VersionChanged, m_model, &SystemInfoModel::setVersion);
    connect(m_systemInfoInter, &__SystemInfo::SystemTypeChanged, m_model, &SystemInfoModel::setType);
    connect(m_systemInfoInter, &__SystemInfo::ProcessorChanged, m_model, &SystemInfoModel::setProcessor);
    connect(m_systemInfoInter, &__SystemInfo::MemoryCapChanged, m_model, &SystemInfoModel::setMemory);
    connect(m_systemInfoInter, &__SystemInfo::DiskCapChanged, m_model, &SystemInfoModel::setDisk);
}

void SystemInfoWork::activate()
{
    m_model->setVersion(m_systemInfoInter->version());
    m_model->setType(m_systemInfoInter->systemType());
    m_model->setProcessor(m_systemInfoInter->processor());
    m_model->setMemory(m_systemInfoInter->memoryCap());
    m_model->setDisk(m_systemInfoInter->diskCap());

    m_model->setBootDelay(m_dbusGrub->timeout() > 1);
    m_model->setThemeEnabled(m_dbusGrub->enableTheme());

    QDBusPendingCall call = m_dbusGrub->GetSimpleEntryTitles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<QStringList> reply = call.reply();
            QStringList entries = reply.value();
            m_model->setEntryLists(entries);
            m_model->setDefaultEntry(m_dbusGrub->defaultEntry());
        } else {
            qWarning() << "get grub entry list failed : " << call.error().message();
        }
    });
}

void SystemInfoWork::deactivate()
{

}

void SystemInfoWork::setBootDelay(bool value)
{
    m_dbusGrub->setTimeout(value ? 5 : 1);
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
