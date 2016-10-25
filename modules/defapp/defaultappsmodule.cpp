#include "defaultappsmodule.h"

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_defAppModel(new DefAppModel(this)),
      m_defAppWorker(new DefAppWorker(m_defAppModel,this)),
      m_defaultappsWidget(new DefaultAppsWidget),
      m_defaultAppsDetail(nullptr)
{
    connect(m_defaultappsWidget, &DefaultAppsWidget::showDefaultAppsDetail, this, &DefaultAppsModule::showDefaultAppsDetail);
}

void DefaultAppsModule::initialize()
{

}

void DefaultAppsModule::moduleActive()
{
    //    m_defaultAppsInter->blockSignals(false);
    //    m_defaultAppsInter->getAllProperties();
    qDebug()<<"Active";
}

//模块非活动时禁用dbus
void DefaultAppsModule::moduleDeactive()
{
//    if (m_defaultappsWidget)
//    {
//        m_defaultappsWidget->deleteLater();
//        m_defaultappsWidget = nullptr;
//    }
    qDebug()<<"Deactive";
    //    m_defaultAppsInter->blockSignals(true);
}

ModuleWidget *DefaultAppsModule::moduleWidget()
{
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
        connect(m_defaultAppsDetail,&DefAppViewer::SetDefaultApp,m_defAppWorker,&DefAppWorker::onSetDefaultAppChanged); //设置默认程序
        connect(m_defaultAppsDetail,&DefAppViewer::Reset,m_defAppWorker,&DefAppWorker::onResetChanged);  //恢复默认
    }
    //    m_defaultAppsDetail->refresh();

    m_frameProxy->pushWidget(this, m_defaultAppsDetail);
}

void DefaultAppsModule::contentPopped(ContentWidget * const w)
{
    if (w == m_defaultAppsDetail)
        m_defaultAppsDetail = nullptr;

    w->deleteLater();
}

