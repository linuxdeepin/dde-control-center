#ifndef WACOMSETTINGS_H
#define WACOMSETTINGS_H

#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>


class QSlider;
namespace dcc {
namespace widgets {
class SettingsGroup;
class TitledSliderItem;
}
namespace wacom {
class WacomModelBase;
class WacomSettings : public dcc::widgets::TranslucentFrame
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
    dcc::widgets::SettingsGroup *m_mainGroup;
    WacomModelBase *m_baseSettings;
    widgets::TitledSliderItem *m_pressureSlider;
    QSlider *m_preSlider;
};

}
}

#endif // WACOMSETTINGS_H
