#include "syncmodule.h"
#include "syncwidget.h"
#include "syncworker.h"
#include "syncmodel.h"
#include "syncstatewidget.h"

#include <QThread>

using namespace dcc;
using namespace dcc::sync;

SyncModule::SyncModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_moduleWidget(nullptr)
{

}

SyncModule::~SyncModule()
{
}

void SyncModule::initialize()
{
    m_model = new SyncModel;
    m_worker = new SyncWorker(m_model);

    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

const QString SyncModule::name() const
{
    return QStringLiteral("deepin ID");
}

void SyncModule::moduleActive()
{
    m_worker->activate();
}

void SyncModule::moduleDeactive()
{
    m_worker->deactivate();
}

void SyncModule::reset()
{

}

ModuleWidget *SyncModule::moduleWidget()
{
    if (!m_moduleWidget) {
        m_moduleWidget = new SyncWidget;
        m_moduleWidget->setModel(m_model);

        connect(m_moduleWidget, &SyncWidget::requestLogin, m_worker, &SyncWorker::loginUser);
        connect(m_moduleWidget, &SyncWidget::requestShowSyncDetails, this, &SyncModule::ShowSyncDetails);
    }

    return m_moduleWidget;
}

void SyncModule::contentPopped(ContentWidget * const w)
{

}

void SyncModule::ShowSyncDetails()
{
    SyncStateWidget* detailPage = new SyncStateWidget;
    detailPage->setModel(m_model);

    connect(detailPage, &SyncStateWidget::requestEnableSync, m_worker, &SyncWorker::setAutoSync);
    connect(detailPage, &SyncStateWidget::requestSetModuleState, m_worker, &SyncWorker::setSync);

    m_frameProxy->pushWidget(this, detailPage);
}
