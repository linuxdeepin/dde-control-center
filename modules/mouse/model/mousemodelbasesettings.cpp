#include "mousemodelbasesettings.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModelBaseSettings::MouseModelBaseSettings(QObject *parent)
    :MouseModelBase(parent)
{

}

void MouseModelBaseSettings::setLeftHandState(const bool state)
{
    if (m_leftHandState != state) {
        m_leftHandState = state;
        emit leftHandStateChanged(state);
    }
}

void MouseModelBaseSettings::setDisIfTyping(const bool state)
{
    if (m_disIfTyping != state) {
        m_disIfTyping = state;
        emit disIfTypingStateChanged(state);
    }
}
