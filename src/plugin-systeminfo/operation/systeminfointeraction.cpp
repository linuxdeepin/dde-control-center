#include "systeminfointeraction.h"

#include "dccfactory.h"

#include <QtQml/qqml.h>

using namespace  dccV25;

SystemInfoInteraction::SystemInfoInteraction(QObject *parent)
    : QObject{ parent }
    , m_systemInfoMode(nullptr)
    , m_systemInfoWork(nullptr)
{
    qmlRegisterType<SystemInfoWork>("org.deepin.dcc.systemInfo", 1, 0, "SystemInfoWork");
    qmlRegisterType<SystemInfoModel>("org.deepin.dcc.systemInfo", 1, 0, "SystemInfoModel");

    m_systemInfoMode = new SystemInfoModel(this);
    m_systemInfoWork = new SystemInfoWork(m_systemInfoMode, this);

    m_systemInfoWork->activate();
}

SystemInfoWork *SystemInfoInteraction::systemInfoWork() const
{
    return m_systemInfoWork;
}

void SystemInfoInteraction::setSystemInfoWork(SystemInfoWork *newSystemInfoWork)
{
    m_systemInfoWork = newSystemInfoWork;
}

SystemInfoModel *SystemInfoInteraction::systemInfoMode() const
{
    return m_systemInfoMode;
}

void SystemInfoInteraction::setSystemInfoMode(SystemInfoModel *newSystemInfoMode)
{
    m_systemInfoMode = newSystemInfoMode;
}

DCC_FACTORY_CLASS(SystemInfoInteraction)


#include "systeminfointeraction.moc"
