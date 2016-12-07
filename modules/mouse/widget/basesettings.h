#ifndef BASESETTINGS_H
#define BASESETTINGS_H

#include "settingsgroup.h"
#include "settingsitem.h"
#include "switchwidget.h"
#include "titledslideritem.h"
#include "../mousemodel.h"
#include "dccslider.h"
#include "translucentframe.h"
#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::widgets;

class BaseSettings : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit BaseSettings(QWidget *parent = 0);

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

private:
    QVBoxLayout   *m_mainLayout;
    SettingsGroup *m_mainGroup;
    SwitchWidget  *m_leftHand;
    SwitchWidget  *m_naturalScroll;
    SwitchWidget  *m_isTyping;
    TitledSliderItem *douSlider;
    MouseModelBaseSettings *m_baseSettings;
};

#endif // BASESETTINGS_H
