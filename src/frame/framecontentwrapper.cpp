#include "framecontentwrapper.h"

FrameContentWrapper::FrameContentWrapper(QWidget *parent)
    : QWidget(parent)
{
//    setStyleSheet("background-color: red;");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
