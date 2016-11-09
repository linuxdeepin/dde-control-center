#ifndef THINKPADSETTINGS_H
#define THINKPADSETTINGS_H

#include "settingsitem.h"
#include "speedslider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include <QObject>

class ThinkpadSettings : public SettingsItem
{
    Q_OBJECT
public:
    explicit ThinkpadSettings(QObject *parent = 0);
    void setModel(MouseModelThinkpadSettings * const baseSettings);

signals:
    void Reset();
    void requestSetSliderValue(const int &value);

public slots:
    void setSliderValue(const int &value);
    void setSliderMaxValue(const int &value);
    void setSliderMinValue(const int &value);
    void setSliderStep(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    SettingsGroup *m_mainGroup;
    MouseModelThinkpadSettings *m_baseSettings;
    SpeedSlider *speedSlider;
};

#endif // THINKPADSETTINGS_H
