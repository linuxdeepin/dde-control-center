#include "defaultappsmodule.h"

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_defaultappsWidget(nullptr),
      m_defaultAppsDetail(nullptr)
{

}

DefaultAppsModule::~DefaultAppsModule()
{
    m_defAppModel->deleteLater();
    m_defAppWorker->deleteLater();
}

void DefaultAppsModule::initialize()
{
    m_defAppModel  = new DefAppModel;
    m_defAppWorker = new DefAppWorker(m_defAppModel);
    m_defAppModel->moveToThread(qApp->thread());
    m_defAppWorker->moveToThread(qApp->thread());
}

void DefaultAppsModule::moduleActive()
{
    m_defAppWorker->active();
}

//模块非活动时禁用dbus
void DefaultAppsModule::moduleDeactive()
{
    m_defAppWorker->deactive();
}

ModuleWidget *DefaultAppsModule::moduleWidget()
{
    if (!m_defaultappsWidget) {
        m_defaultappsWidget = new DefaultAppsWidget;
        connect(m_defaultappsWidget, &DefaultAppsWidget::showDefaultAppsDetail, this, &DefaultAppsModule::showDefaultAppsDetail);
    }
    return m_defaultappsWidget;
}

const QString DefaultAppsModule::name() const
{
    return "DefaultApplications";
}

//show下级菜单
void DefaultAppsModule::showDefaultAppsDetail()
{
    if (!m_defaultAppsDetail) {
        m_defaultAppsDetail = new DefAppViewer;
        m_defaultAppsDetail->setModel(m_defAppModel);
        connect(m_defaultAppsDetail, &DefAppViewer::SetDefaultApp,   m_defAppWorker, &DefAppWorker::onSetDefaultAppChanged); //设置默认程序
        connect(m_defaultAppsDetail, &DefAppViewer::Reset,           m_defAppWorker, &DefAppWorker::onResetTriggered);  //恢复默认
        connect(m_defaultAppsDetail, &DefAppViewer::autoOpenChanged, m_defAppWorker, &DefAppWorker::onAutoOpenChanged);

    }
    m_frameProxy->pushWidget(this, m_defaultAppsDetail);
}

void DefaultAppsModule::contentPopped(ContentWidget * const w)
{
    if (w == m_defaultAppsDetail)
        m_defaultAppsDetail = nullptr;

    w->deleteLater();
}

