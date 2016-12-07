#include "mousemodule.h"

MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_mouseWidget(nullptr)
{
}

void MouseModule::initialize()
{
    m_model  = new MouseModel;
    m_worker = new MouseWorker(m_model);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void MouseModule::moduleActive()
{
    m_worker->active();
}

void MouseModule::moduleDeactive()
{
    m_worker->deactive();
}

void MouseModule::reset()
{
    m_worker->onDefaultReset();
}

ModuleWidget *MouseModule::moduleWidget()
{
    if (!m_mouseWidget) {
        m_mouseWidget = new MouseWidget;
        m_mouseWidget->setModel(m_model);
        connect(m_mouseWidget, &MouseWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetNaturalScroll, m_worker, &MouseWorker::onNaturalScrollStateChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
        connect(m_mouseWidget, &MouseWidget::setDefault, m_worker, &MouseWorker::onDefaultReset);
        connect(m_mouseWidget, &MouseWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
    }
    return m_mouseWidget;
}

const QString MouseModule::name() const
{
    return "Mouse and TouchPad";
}

MouseModule::~MouseModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();

    if (m_mouseWidget) {
        m_mouseWidget->deleteLater();
    }
}

void MouseModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
