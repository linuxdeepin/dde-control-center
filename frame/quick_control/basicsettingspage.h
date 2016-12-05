#ifndef BASICSETTINGSPAGE_H
#define BASICSETTINGSPAGE_H

#include "dccslider.h"

#include <QWidget>

class BasicSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit BasicSettingsPage(QWidget *parent = 0);

private:
    dcc::DCCSlider *m_soundSlider;
    dcc::DCCSlider *m_lightSlider;
};

#endif // BASICSETTINGSPAGE_H
