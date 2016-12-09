#ifndef WACOMWIDGET_H
#define WACOMWIDGET_H

#include "modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc
{
class SettingsGroup;
namespace wacom
{
class WacomSettings;
class WacomModel;
class WacomWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit WacomWidget();
    void setModel(WacomModel *const model);

signals:
    void requestSetPressureValue(const int value);

private:
    SettingsGroup *m_userGroup;
    WacomSettings      *m_wacomSettings;
};
}
}


#endif // WACOMWIDGET_H
