#ifndef WACOMWIDGET_H
#define WACOMWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include  "widget/wacomsettings.h"
#include "wacommodel.h"
#include <QPushButton>
#include <QVBoxLayout>

class WacomWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit WacomWidget();
    void setModel(WacomModel *const model);

signals:
    void requestSetPressureValue(const int value);

private:
    dcc::SettingsGroup *m_userGroup;
    WacomSettings      *m_wacomSettings;
};

#endif // WACOMWIDGET_H
