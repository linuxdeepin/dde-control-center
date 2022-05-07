/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BRIGHTNESSWIDGET_H
#define BRIGHTNESSWIDGET_H

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"

#include <DTipLabel>
#include <DSwitchButton>

#include <QWidget>

class QLabel;
class QVBoxLayout;
class QSpacerItem;

namespace DCC_NAMESPACE {

class TipsLabel;
class SwitchWidget;
class SettingsGroup;
class TitledSliderItem;
class Monitor;
class DisplayModel;

class BrightnessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BrightnessWidget(QWidget *parent = nullptr);
    ~BrightnessWidget();

public:
    void setMode(DisplayModel *model);
    void setAdjustCCTmode(int mode);
    void showBrightness(Monitor *monitor = nullptr);

Q_SIGNALS:
    void requestSetMonitorBrightness(Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetColorTemperature(const int value);
    void requestSetMethodAdjustCCT(const int mode);

private:
    void addSlider();
    int colorTemperatureToValue(int kelvin);
    QString brightnessToTickInterval(const double tb) const;
    void setColorTemperatureVisible(bool visible);

private:
    DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
    TitleLabel *m_brightnessTitle;
    QSpacerItem *m_autoLightSpacerItem;
    SwitchWidget *m_autoLightMode;
    QSpacerItem *m_colorSpacerItem;
    QWidget *m_tempratureColorWidget;
    TitleLabel *m_tempratureColorTitle;
    SwitchWidget *m_nightShift;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_nightTips;
    SettingsGroup *m_settingsGroup;
    SwitchWidget *m_nightManual;
    TitledSliderItem *m_cctItem;
    QSpacerItem *m_nightShiftSpacerItem;
    QSpacerItem *m_nightTipsSpacerItem;
    QSpacerItem *m_nightManualSpacerItem;

    int m_miniScales = 0;
    QMap<Monitor *, QWidget *> m_monitorBrightnessMap;
};

}

#endif // BRIGHTNESSWIDGET_H
