#include "powermodule.h"

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
}

void PowerModule::moduleActive()
{
    m_powerWorker->active();
}

void PowerModule::moduleDeactive()
{
    m_powerWorker->deactive();
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
    }

    return m_powerWidget;
}

const QString PowerModule::name() const
{
    return QStringLiteral("power");
}
