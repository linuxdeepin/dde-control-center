#ifndef BASESETTINGS_H
#define BASESETTINGS_H

#include "settingsgroup.h"
#include "settingsitem.h"
#include "switchwidget.h"
#include "speedslider.h"
#include "../mousemodel.h"
#include <QObject>
#include <QVBoxLayout>

using namespace dcc;
class BaseSettings : public SettingsItem
{
    Q_OBJECT
public:
    explicit BaseSettings(QObject *parent = 0);

signals:
    void Reset();
    void requestSetLeftHand(const bool state);
    void requestSetNaturalScroll(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetSliderValue(const int &value);

public slots:
    void setModel(MouseModelBaseSettings * const baseSettings);
    void setLeftHandState(const bool state);
    void setNaturalScroll(const bool state);
    void setDisIfTypingState(const bool state);
    void setSliderValue(const int &value);
    void setSliderMaxValue(const int &value);
    void setSliderMinValue(const int &value);
    void setSliderStep(const int &value);

private:
    QVBoxLayout   *m_mainLayout;
    SettingsGroup *m_mainGroup;
    SwitchWidget  *m_leftHand;
    SwitchWidget  *m_naturalScroll;
    SwitchWidget  *m_isTyping;
    SpeedSlider *douSlider;
    MouseModelBaseSettings *m_baseSettings;
};

#endif // BASESETTINGS_H
