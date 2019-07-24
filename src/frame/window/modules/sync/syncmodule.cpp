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
    , m_mainWidget(nullptr)
    , m_model(nullptr)
    , m_worker(nullptr)
{
}

void SyncModule::initialize()
{
    m_mainWidget = new SyncWidget;
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
    connect(m_mainWidget, &SyncWidget::requestLoginUser, m_worker, &dcc::cloudsync::SyncWorker::loginUser, Qt::UniqueConnection);

    m_mainWidget->setModel(m_model);
    m_worker->activate(); //refresh data

    return m_mainWidget;
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
