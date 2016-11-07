#include "keyboardwork.h"

KeyboardWork::KeyboardWork(QObject *parent)
    : QObject(parent)
{
    m_keyboardInter = new KeyboardInter("com.deepin.daemon.InputDevice.Keyboard",
                                        "/com/deepin/daemon/InputDevice/Keyboard",
                                        QDBusConnection::sessionBus(), this);
}
