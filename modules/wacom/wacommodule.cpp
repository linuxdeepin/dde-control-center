#include "wacomemodule.h"
#include "contentwidget.h"
#include "wacommodel.h"
#include "wacomwidget.h"
#include "wacomworker.h"
using namespace dcc;
using namespace dcc::wacom;
WacomModule::WacomModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_wacomWidget(nullptr)
{
}

void WacomModule::initialize()
{
    m_model  = new WacomModel;
    m_worker = new WacomWorker(m_model);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void WacomModule::moduleActive()
{
    m_worker->active();
}

void WacomModule::moduleDeactive()
{
    m_worker->deactive();
}

void WacomModule::reset()
{

}

ModuleWidget *WacomModule::moduleWidget()
{
    if (!m_wacomWidget)
    {
        m_wacomWidget = new WacomWidget;

        connect(m_wacomWidget, &WacomWidget::requestSetPressureValue, m_worker, &WacomWorker::onPressureSensitiveChanged);
        connect(m_model, &WacomModel::existChanged, [this](const bool exist) { m_frameProxy->setModuleVisible(this, exist); });

        m_wacomWidget->setModel(m_model);
        m_frameProxy->setModuleVisible(this, m_model->exist());
    }

    return m_wacomWidget;
}

const QString WacomModule::name() const
{
    return QStringLiteral("wacom");
}

WacomModule::~WacomModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void WacomModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
