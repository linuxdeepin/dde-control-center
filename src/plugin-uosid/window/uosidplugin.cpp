#include "syncwidget.h"
#include "uosidplugin.h"

#include <QVBoxLayout>
#include <syncmodel.h>
#include <syncworker.h>

QString UosIDPlugin::name() const
{
    return QStringLiteral("UOSID");
}

ModuleObject *UosIDPlugin::module()
{
    // 一级界面
    UosIDModule *uosidInterface = new UosIDModule;
    uosidInterface->setChildType(ModuleObject::ChildType::Page);

    // UOSID 展示页面
    UosIDDetailsModule *uosIdWidget = new  UosIDDetailsModule(uosidInterface->model(), uosidInterface->work(), uosidInterface);
    uosidInterface->appendChild(uosIdWidget);
    return uosidInterface;
}

UosIDModule::UosIDModule(QObject *parent)
    : ModuleObject("UOSID", tr("UOSID"), tr("UOSID"), QIcon::fromTheme("dcc_nav_cloudsync"), parent)
    , m_model(new SyncModel(this))
    , m_work(new SyncWorker(m_model, this))
{

}

UosIDModule::~UosIDModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void UosIDModule::active()
{
    m_work->activate();
}

QWidget *UosIDDetailsModule::page()
{
    SyncWidget *widget = new SyncWidget;

    connect(widget, &SyncWidget::requestLoginUser, m_worker, &SyncWorker::loginUser, Qt::UniqueConnection);

    connect(widget, &SyncWidget::requestAsyncLogoutUser, m_worker, &SyncWorker::logoutUser, Qt::QueuedConnection);
    connect(widget, &SyncWidget::requestPullMessage, m_worker, &SyncWorker::onPullMessage);
    connect(widget, &SyncWidget::requestSetFullname, m_worker, &SyncWorker::onSetFullname);

    connect(widget, &SyncWidget::requestBindAccount, m_worker, &SyncWorker::asyncBindAccount, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestUnBindAccount, m_worker, &SyncWorker::asyncUnbindAccount, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestSetAutoSync, m_worker, &SyncWorker::setAutoSync, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestLocalBindCheck, m_worker, &SyncWorker::asyncLocalBindCheck, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestSetModuleState, m_worker, &SyncWorker::setSyncState, Qt::QueuedConnection);

    connect(widget, &SyncWidget::requestUOSID, m_worker, &SyncWorker::getUOSID, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestUUID, m_worker, &SyncWorker::getUUID, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestHostName, m_worker, &SyncWorker::getHostName, Qt::UniqueConnection);
    widget->setModel(m_model);

    return widget;
}
