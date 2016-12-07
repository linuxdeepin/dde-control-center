#ifndef WACOMSETTINGS_H
#define WACOMSETTINGS_H
#include "settingsitem.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../wacommodel.h"
#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>
using namespace dcc;
using namespace dcc::widgets;
class WacomSettings : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit WacomSettings(QWidget *parent = 0);
    void setModel(WacomModelBase *const baseSettings);

signals:
    void requestSetPressureValue(const int &value);

public slots:
    void setPressureValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    SettingsGroup *m_mainGroup;
    WacomModelBase *m_baseSettings;
    TitledSliderItem *m_pressureSlider;
    QSlider *m_preSlider;
};

#endif // WACOMSETTINGS_H
