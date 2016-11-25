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

private:
    int delayToSliderValue(const int min, const int max, const int delay) const;
    QString delayToLiteralString(const int delay) const;

private slots:
    void setScreenBlackDelay(const int delay);
    void setSleepDelay(const int delay);

private:
    dcc::SettingsGroup *m_sleepTimeoutSettings;
    dcc::widgets::TitledSliderItem *m_monitorSleep;
    dcc::widgets::TitledSliderItem *m_computerSleep;
    dcc::SettingsGroup *m_passwordSettings;
    dcc::SwitchWidget *m_displayNeedPassword;
    dcc::SwitchWidget *m_wakeNeedPassword;
    dcc::SettingsGroup *m_notebookSettings;
    dcc::SwitchWidget *m_sleepOnLidOff;
};

#endif // POWERWIDGET_H
