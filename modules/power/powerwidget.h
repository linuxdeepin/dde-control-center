#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include "modulewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "powermodel.h"

class PowerWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit PowerWidget();

    void setModel(PowerModel * const model);

signals:
    void requestSetScreenBlackLock(const bool lock) const;

private:
    dcc::SettingsGroup *m_suspendSettings;
    dcc::SwitchWidget *m_displayNeedPassword;
    dcc::SwitchWidget *m_wakeNeedPassword;
};

#endif // POWERWIDGET_H
