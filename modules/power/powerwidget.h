#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include "modulewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "powermodel.h"

namespace dcc {
namespace widgets {
class TitledSliderItem;
}
}

class QSlider;
class PowerWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit PowerWidget();

    void setModel(PowerModel * const model);

signals:
    void requestSetScreenBlackLock(const bool lock) const;
    void requestSetSleepLock(const bool lock) const;
    void requestSetSleepOnLidClosed(const bool sleep) const;
    void requestSetScreenBlackDelay(const int delay) const;
    void requestSetSleepDelay(const int delay) const;

private:
    int delayToSliderValue(const int min, const int max, const int delay) const;
    int sliderValueToDelay(const int min, const int max, const int tickInterval, const int value) const;
    QString delayToLiteralString(const int delay) const;

private slots:
    void setScreenBlackDelay(const int delay);
    void setSleepDelay(const int delay);

    void monitorSliderValueChanged(const int delay);
    void sleepSliderValueChanged(const int delay);

private:
    dcc::SettingsGroup *m_sleepTimeoutSettings;
    dcc::widgets::TitledSliderItem *m_monitorSleep;
    dcc::widgets::TitledSliderItem *m_computerSleep;
    dcc::SettingsGroup *m_passwordSettings;
    dcc::SwitchWidget *m_displayNeedPassword;
    dcc::SwitchWidget *m_wakeNeedPassword;
    dcc::SettingsGroup *m_notebookSettings;
    dcc::SwitchWidget *m_sleepOnLidOff;
    QSlider *m_monitorSlider;
    QSlider *m_sleepSlider;
};

#endif // POWERWIDGET_H
