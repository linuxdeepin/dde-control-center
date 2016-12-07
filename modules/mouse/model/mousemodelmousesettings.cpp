#include "mousemodelmousesettings.h"

MouseModelMouseSettings::MouseModelMouseSettings(QObject *parent)
    :MouseModelBase(parent)
{

}

void MouseModelMouseSettings::setSwitchState(const bool state)
{
    m_switchValue = state;
    emit switchChanged(state);
}
