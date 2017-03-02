#ifndef BRIGHTNESSITEM_H
#define BRIGHTNESSITEM_H

#include "settingsitem.h"

#include <QSlider>

#include <dimagebutton.h>

namespace dcc {

namespace display {

class Monitor;
class BrightnessItem : public widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit BrightnessItem(QWidget *parent = 0);

signals:
    void requestSetMonitorBrightness(Monitor *mon, const double brightness) const;

public slots:
    void setValue(const double value);
    void setMonitor(Monitor * const mon);

private:
    Dtk::Widget::DImageButton *m_leftIcon;
    Dtk::Widget::DImageButton *m_rightIcon;
    QSlider *m_brightnessSlider;

    Monitor *m_monitor;
};

}

}

#endif // BRIGHTNESSITEM_H
