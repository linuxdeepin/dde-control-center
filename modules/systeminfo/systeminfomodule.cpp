#include "systeminfomodule.h"
#include "copyrightwidget.h"
#include "bootwidget.h"

namespace dcc{
namespace systeminfo{

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_bootWidget(nullptr),
      m_mainWidget(nullptr),
      m_copyrightWidget(nullptr)

{
}

SystemInfoModule::~SystemInfoModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if(m_mainWidget)
        m_mainWidget->deleteLater();
}

void SystemInfoModule::initialize()
{
    m_model = new SystemInfoModel();
    m_work = new SystemInfoWork(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void SystemInfoModule::moduleActive()
{
    m_work->activate();
}

void SystemInfoModule::moduleDeactive()
{
    m_work->deactivate();
}

void SystemInfoModule::reset()
{

}

ModuleWidget *SystemInfoModule::moduleWidget()
{
    if(!m_mainWidget)
    {
        m_mainWidget = new SystemInfoWidget(m_model);
        connect(m_mainWidget, SIGNAL(copyright()), this, SLOT(onPushCopyright()));
        connect(m_mainWidget, SIGNAL(boot()), this, SLOT(onPushBoot()));
    }

    return m_mainWidget;
}

const QString SystemInfoModule::name() const
{
    return QStringLiteral("system_info");
}

void SystemInfoModule::contentPopped(ContentWidget * const w)
{
    if(w == m_bootWidget)
        m_bootWidget = nullptr;
    else if(w == m_copyrightWidget)
        m_copyrightWidget = nullptr;

    w->deleteLater();
}

void SystemInfoModule::onPushCopyright()
{
    if(!m_copyrightWidget)
    {
        m_copyrightWidget = new CopyrightWidget();
    }

    m_frameProxy->pushWidget(this, m_copyrightWidget);
}

void SystemInfoModule::onPushBoot()
{
    if(!m_bootWidget)
    {
        m_bootWidget = new BootWidget;
        m_bootWidget->setModel(m_model);

        connect(m_bootWidget, SIGNAL(bootdelay(bool)), m_work, SLOT(setBootDelay(bool)));
        connect(m_bootWidget, SIGNAL(enableTheme(bool)),m_work, SLOT(setEnableTheme(bool)));
        connect(m_bootWidget, SIGNAL(defaultEntry(QString)), m_work, SLOT(setDefaultEntry(QString)));
    }

    m_frameProxy->pushWidget(this, m_bootWidget);
}

}
}
