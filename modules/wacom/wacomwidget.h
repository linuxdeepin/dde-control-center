#ifndef WACOMWIDGET_H
#define WACOMWIDGET_H

#include "modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc{
namespace widgets {
class NextPageWidget;
}
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
    void requestShowMode();

private:
    const QString getValue(const bool mode);

private slots:
    void onCursorModeChanged(const bool modeChanged);

private:
    WacomSettings      *m_wacomSettings;
    dcc::widgets::NextPageWidget *m_selectMode;
};
}
}


#endif // WACOMWIDGET_H
