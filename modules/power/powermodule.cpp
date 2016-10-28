#include "powermodule.h"

PowerModule::PowerModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_powerModel(new PowerModel(this)),
      m_powerWidget(nullptr),
      m_powerWorker(new PowerWorker(m_powerModel, this))
{

}

void PowerModule::initialize()
{

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
