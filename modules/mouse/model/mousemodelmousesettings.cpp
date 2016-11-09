#include "mousemodelmousesettings.h"

MouseModelMouseSettings::MouseModelMouseSettings(QObject *parent)
{
    Q_UNUSED(parent);

}

void MouseModelMouseSettings::setSwitchState(const bool state)
{
    m_switchValue = state;
    emit switchChanged(state);
}
