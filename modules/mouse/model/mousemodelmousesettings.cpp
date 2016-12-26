#include "mousemodelmousesettings.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModelMouseSettings::MouseModelMouseSettings(QObject *parent)
    :MouseModelBase(parent)
{

}

void MouseModelMouseSettings::setSwitchState(const bool state)
{
    m_switchValue = state;
    emit switchChanged(state);
}
