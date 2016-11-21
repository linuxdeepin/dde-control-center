#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H
#include "settingsitem.h"
#include "speedslider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include <QObject>
#include <QVBoxLayout>

const int MouseMoveSpeedMax  = 3000;
const int MouseMoveSpeedMin  = 200;
const int MouseMoveSpeedStep = 400;

using namespace dcc;
class MouseSettings : public SettingsItem
{
    Q_OBJECT
public:
    explicit MouseSettings(QObject *parent = 0);
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
    SettingsGroup *m_mainGroup;
    MouseModelMouseSettings *m_baseSettings;
    SpeedSlider *speedSlider;
    SwitchWidget *switchWidget;
};

#endif // MOUSESETTINGS_H
