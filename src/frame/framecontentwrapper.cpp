#include "framecontentwrapper.h"

FrameContentWrapper::FrameContentWrapper(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
