#include "datetime.h"

Datetime::Datetime(dde::FrameProxyInterface *frame)
    :ModuleWidget(frame)
{
    setTitle(tr("Time and Date"));
}
