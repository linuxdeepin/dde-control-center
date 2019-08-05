#include "syncmodule.h"

#include "interfaces/frameproxyinterface.h"
#include "../../../modules/sync/syncworker.h"
#include "../../../modules/sync/syncmodel.h"

#include "syncwidget.h"

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
    m_model = new dcc::cloudsync::SyncModel;
    m_worker = new dcc::cloudsync::SyncWorker(m_model);
}

void SyncModule::reset() {}

const QString SyncModule::name() const
{
    return QStringLiteral("cloudsync");
}

void SyncModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *SyncModule::moduleWidget()
{
    SyncWidget* widget = new SyncWidget;
    connect(widget, &SyncWidget::requestLoginUser, m_worker, &dcc::cloudsync::SyncWorker::loginUser, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestSetAutoSync, m_worker, &dcc::cloudsync::SyncWorker::setAutoSync, Qt::UniqueConnection);
    connect(widget, &SyncWidget::requestLogoutUser, m_worker, &dcc::cloudsync::SyncWorker::logoutUser, Qt::QueuedConnection);
    connect(widget, &SyncWidget::requestSetModuleState, m_worker, &dcc::cloudsync::SyncWorker::setSync, Qt::UniqueConnection);

    widget->setModel(m_model);
    m_worker->activate(); //refresh data

    return widget;
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
