#include "syncmodule.h"

#include "interfaces/frameproxyinterface.h"
#include "../../../modules/sync/syncworker.h"
#include "../../../modules/sync/syncmodel.h"

#include "syncwidget.h"
#include "widgets/utils.h"
#include "window/utils.h"
#include "window/mainwindow.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncModule::SyncModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_worker(nullptr)
{
}

void SyncModule::initialize()
{
    m_worker = new dcc::cloudsync::SyncWorker(m_model);
}

const QString SyncModule::name() const
{
    return QStringLiteral("cloudsync");
}

const QString SyncModule::displayName() const
{
    return tr("Union ID");
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void SyncModule::active()
{
    SyncWidget *widget = new SyncWidget;
    connect(widget, &SyncWidget::requestLoginUser, m_worker, &dcc::cloudsync::SyncWorker::loginUser, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestSetAutoSync, m_worker, &dcc::cloudsync::SyncWorker::setAutoSync, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestLogoutUser, m_worker, &dcc::cloudsync::SyncWorker::logoutUser, Qt::QueuedConnection);
    connect(widget, &SyncWidget::requestSetModuleState, m_worker, &dcc::cloudsync::SyncWorker::setSync, Qt::UniqueConnection);
    MainWindow *pMainWindow = static_cast<MainWindow *>(m_frameProxy);
    widget->setModel(m_model, pMainWindow);

    m_frameProxy->pushWidget(this, widget);

    m_worker->activate(); //refresh data
}

void SyncModule::preInitialize()
{
    m_model = new dcc::cloudsync::SyncModel;
    auto req = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.deepinid");

    m_model->setSyncIsValid(req.value() && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));

    m_frameProxy->setModuleVisible(this, m_model->syncIsValid() && !IsServerSystem);
}

QStringList SyncModule::availPage() const
{
    QStringList sl;
    sl << "Cloud Sync" << "Sync ID Sign In";
    return sl;
}
