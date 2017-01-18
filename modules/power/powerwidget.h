#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include "modulewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "powermodel.h"


class QSlider;

namespace dcc {
namespace widgets {
class TitledSliderItem;
}

namespace power {
class PowerWidget : public dcc::ModuleWidget
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
    QString delayToLiteralString(const int delay) const;

private slots:
    void setScreenBlackDelay(const int delay);
    void setSleepDelay(const int delay);

private:
    widgets::SettingsGroup *m_sleepTimeoutSettings;
    widgets::TitledSliderItem *m_monitorSleep;
    widgets::TitledSliderItem *m_computerSleep;
    widgets::SettingsGroup *m_passwordSettings;
    widgets::SwitchWidget *m_displayNeedPassword;
    widgets::SwitchWidget *m_wakeNeedPassword;
    widgets::SettingsGroup *m_notebookSettings;
    widgets::SwitchWidget *m_sleepOnLidOff;
    QSlider *m_monitorSlider;
    QSlider *m_sleepSlider;
};
}

}



#endif // POWERWIDGET_H
