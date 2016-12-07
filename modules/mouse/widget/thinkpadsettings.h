#ifndef THINKPADSETTINGS_H
#define THINKPADSETTINGS_H

#include "settingsitem.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include "translucentframe.h"
#include "titledslideritem.h"
#include "settingshead.h"
#include "dccslider.h"
#include <QObject>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::widgets;

class ThinkpadSettings : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit ThinkpadSettings(QWidget *parent = 0);
    void setModel(MouseModelThinkpadSettings * const baseSettings);

signals:
    void requestSetSliderValue(const int &value);

public slots:
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    SettingsGroup *m_mainGroup;
    MouseModelThinkpadSettings *m_baseSettings;
    TitledSliderItem *m_speedSlider;
    SettingsHead *m_title;
    QSlider *m_spSlider;
};

#endif // THINKPADSETTINGS_H
