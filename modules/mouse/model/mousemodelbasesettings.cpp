#include "mousemodelbasesettings.h"

MouseModelBaseSettings::MouseModelBaseSettings(QObject *parent)
    :MouseModelBase(parent)
{

}

void MouseModelBaseSettings::setLeftHandState(const bool state)
{
    m_leftHandState = state;
    emit leftHandStateChanged(state);
}

void MouseModelBaseSettings::setNaturalScroll(const bool state)
{
    m_naturalScroll = state;
    emit naturalScrollChanged(state);
}

void MouseModelBaseSettings::setDisIfTyping(const bool state)
{
    m_disIfTyping = state;
    emit disIfTypingStateChanged(state);
}
