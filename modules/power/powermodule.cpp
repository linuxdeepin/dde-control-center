#include "powermodule.h"
#include "powerwidget.h"
#include "powerworker.h"
#include "powermodel.h"

using namespace dcc;
using namespace dcc::power;

PowerModule::PowerModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_powerWidget(nullptr)
{

}

PowerModule::~PowerModule()
{
    m_powerModel->deleteLater();
    m_powerWorker->deleteLater();
}

void PowerModule::initialize()
{
    m_powerModel = new PowerModel;
    m_powerWorker = new PowerWorker(m_powerModel);

    m_powerModel->moveToThread(qApp->thread());
    m_powerWorker->moveToThread(qApp->thread());
}

void PowerModule::moduleActive()
{
    m_powerWorker->active();
}

void PowerModule::moduleDeactive()
{
    m_powerWorker->deactive();
}

void PowerModule::reset()
{

}

void PowerModule::contentPopped(ContentWidget * const w)
{
    Q_UNUSED(w);
}

ModuleWidget *PowerModule::moduleWidget()
{
    if (!m_powerWidget)
    {
        m_powerWidget = new PowerWidget;
        m_powerWidget->setModel(m_powerModel);

        connect(m_powerWidget, &PowerWidget::requestSetScreenBlackLock, m_powerWorker, &PowerWorker::setScreenBlackLock);
        connect(m_powerWidget, &PowerWidget::requestSetSleepLock, m_powerWorker, &PowerWorker::setSleepLock);
        connect(m_powerWidget, &PowerWidget::requestSetSleepOnLidClosed, m_powerWorker, &PowerWorker::setSleepOnLidClosed);
        connect(m_powerWidget, &PowerWidget::requestSetScreenBlackDelay, m_powerWorker, &PowerWorker::setScreenBlackDelay);
        connect(m_powerWidget, &PowerWidget::requestSetSleepDelay, m_powerWorker, &PowerWorker::setSleepDelay);
    }

    return m_powerWidget;
}

const QString PowerModule::name() const
{
    return QStringLiteral("power");
}
