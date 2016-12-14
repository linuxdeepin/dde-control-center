#include "wacommodelbase.h"

using namespace dcc;
using namespace dcc::wacom;

WacomModelBase::WacomModelBase(QObject *parent)
{
    Q_UNUSED(parent);
}

void WacomModelBase::setPressureValue(const int &value)
{
    m_pressureValue = value;
    emit pressureValueChanged(value);
}
