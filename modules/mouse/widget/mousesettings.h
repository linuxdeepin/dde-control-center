#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H
#include "settingsitem.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include "translucentframe.h"
#include "titledslideritem.h"
#include "settingshead.h"
#include "dccslider.h"
#include <QWidget>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::widgets;
class MouseSettings : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit MouseSettings(const QString &title, QWidget *parent = 0);
    void setModel(MouseModelMouseSettings *const baseSettings);

signals:
    void setDefault();
    void requestSetSwitch(const bool state);
    void requestSetSliderValue(const int &value);

public slots:
    void setSwitchTitle(const QString &title);
    void setSwitchState(const bool state);
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    SettingsHead   *m_title;
    SettingsGroup *m_mainGroup;
    MouseModelMouseSettings *m_baseSettings;
    TitledSliderItem *m_speedSlider;
    SwitchWidget *m_switchWidget;
    QSlider *m_spSlider;
};

#endif // MOUSESETTINGS_H
