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

    m_model->setDefaultEntry(m_work->defaultEntry());
    m_model->setEntryLists(m_work->entryLists());
    m_model->setVersion(m_work->version());
    m_model->setType(m_work->type());
    m_model->setProcessor(m_work->processor());
    m_model->setMemory(m_work->memory());
    m_model->setDisk(m_work->disk());

    connect(m_work, SIGNAL(VersionChanged(QString)), m_model, SLOT(setVersion(QString)));
    connect(m_work,SIGNAL(SystemTypeChanged(qlonglong)), m_model, SLOT(setType(qlonglong)));
    connect(m_work, SIGNAL(ProcessorChanged(QString)), m_model, SLOT(setProcessor(QString)));
    connect(m_work, SIGNAL(MemoryCapChanged(qulonglong)), m_model, SLOT(setMemory(qulonglong)));
    connect(m_work, SIGNAL(DiskCapChanged(qulonglong)), m_model, SLOT(setDisk(qulonglong)));

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void SystemInfoModule::moduleActive()
{

}

void SystemInfoModule::moduleDeactive()
{

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
    return "System Information";
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
        m_bootWidget = new BootWidget();
        m_bootWidget->setDefaultEntry(m_model->defaultEntry());
        m_bootWidget->setEntryList(m_model->entryLists());

        connect(m_bootWidget, SIGNAL(bootdelay(bool)), m_work, SLOT(setBootDelay(bool)));
        connect(m_bootWidget, SIGNAL(enableTheme(bool)),m_work, SLOT(setEnableTheme(bool)));
        connect(m_bootWidget, SIGNAL(defaultEntry(QString)), m_work, SLOT(setDefaultEntry(QString)));
    }

    m_frameProxy->pushWidget(this, m_bootWidget);
}

}
}
