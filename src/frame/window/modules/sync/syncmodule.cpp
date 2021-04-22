#include "syncmodule.h"

#include "interfaces/frameproxyinterface.h"
#include "../../../modules/sync/syncworker.h"
#include "../../../modules/sync/syncmodel.h"

#include "syncwidget.h"
#include "window/utils.h"
#include "window/mainwindow.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
using namespace dcc::cloudsync;

SyncModule::SyncModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_worker(nullptr)
{
}

void SyncModule::initialize()
{
}

const QString SyncModule::name() const
{
    return QStringLiteral("cloudsync");
}

const QString SyncModule::displayName() const
{
    if (DSysInfo::isCommunityEdition())
        return tr("Deepin ID");

    return tr("Union ID");
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void SyncModule::active()
{
    SyncWidget *widget = new SyncWidget;
    widget->setVisible(false);
    connect(widget, &SyncWidget::requestLoginUser, m_worker, &dcc::cloudsync::SyncWorker::loginUser, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestSetAutoSync, m_worker, &dcc::cloudsync::SyncWorker::setAutoSync, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestLogoutUser, m_worker, &dcc::cloudsync::SyncWorker::logoutUser, Qt::QueuedConnection);
    connect(widget, &SyncWidget::requestSetModuleState, m_worker, &dcc::cloudsync::SyncWorker::setSync, Qt::UniqueConnection);
    MainWindow *pMainWindow = static_cast<MainWindow *>(m_frameProxy);
    widget->setModel(m_model, pMainWindow);

    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);

    m_worker->activate(); //refresh data
}

void SyncModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    if (!DSysInfo::isDeepin()) {
        qInfo() << "module: " << displayName() << " is disable now!";
        m_frameProxy->setModuleVisible(this, false);
        return;
    }

    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
    m_model = new SyncModel;
    m_worker = new SyncWorker(m_model);

    m_frameProxy->setModuleVisible(this, m_model->syncIsValid() && !IsServerSystem);
    connect(m_model, &SyncModel::syncIsValidChanged, this, [=](bool valid) {
        m_frameProxy->setModuleVisible(this, valid && !IsServerSystem);
    });
}

QStringList SyncModule::availPage() const
{
    QStringList sl;
    sl << "Cloud Sync" << "Sync ID Sign In";
    return sl;
}
