#ifndef THINKPADSETTINGS_H
#define THINKPADSETTINGS_H

#include "settingsitem.h"
#include "speedslider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include "translucentframe.h"
#include "moduletitle.h"
#include <QObject>

const int TrackMoveSpeedMax  = 3000;
const int TrackMoveSpeedMin  = 200;
const int TrackMoveSpeedStep = 400;

using namespace dcc;
class ThinkpadSettings : public TranslucentFrame
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

private:
    QVBoxLayout *m_mainLayout;
    ModuleTitle   *m_title;
    SettingsGroup *m_mainGroup;
    MouseModelThinkpadSettings *m_baseSettings;
    SpeedSlider *speedSlider;
};

#endif // THINKPADSETTINGS_H
