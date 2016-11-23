#ifndef WACOMSETTINGS_H
#define WACOMSETTINGS_H
#include "settingsitem.h"
#include "slider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../wacommodel.h"
#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>
using namespace dcc;
const int PressureMaxValue = 10;
const int PressureMinValue = 1;
const int PressureStep     = 1;
class WacomSettings : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit WacomSettings(QObject *parent = 0);
    void setModel(WacomModelBase *const baseSettings);

signals:
    void requestSetPressureValue(const int &value);

public slots:
    void setPressureValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    SettingsGroup *m_mainGroup;
    WacomModelBase *m_baseSettings;
    WacomPressure *m_pressureSlider;
};

#endif // WACOMSETTINGS_H
