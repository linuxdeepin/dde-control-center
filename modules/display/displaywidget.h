#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "modulewidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"

#include <QPushButton>

namespace dcc {

namespace display {

class DisplayWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit DisplayWidget();

private:
    NextPageWidget *m_resolution;
    QPushButton *m_rotate;
    QPushButton *m_custom;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
