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
