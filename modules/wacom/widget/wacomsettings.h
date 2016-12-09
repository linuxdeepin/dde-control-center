#ifndef WACOMSETTINGS_H
#define WACOMSETTINGS_H

#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>


class QSlider;
namespace dcc {
class SettingsGroup;

namespace widgets {
class TitledSliderItem;
}
namespace wacom {
class WacomModelBase;
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
    widgets::TitledSliderItem *m_pressureSlider;
    QSlider *m_preSlider;
};

}
}

#endif // WACOMSETTINGS_H
