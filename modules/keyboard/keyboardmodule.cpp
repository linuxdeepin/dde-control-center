#include "keyboardmodule.h"

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_keyboardWidget(nullptr)
{

}

void KeyboardModule::initialize()
{
    m_work = new KeyboardWork(this);
    m_model = new KeyboardModel(this);


    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void KeyboardModule::moduleActive()
{

}

void KeyboardModule::moduleDeactive()
{

}

ModuleWidget *KeyboardModule::moduleWidget()
{

}

const QString KeyboardModule::name() const
{
    return "Keyboard";
}

void KeyboardModule::contentPopped(ContentWidget * const w)
{

}

KeyboardModule::~KeyboardModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if(m_keyboardWidget)
        m_keyboardWidget->deleteLater();
}
