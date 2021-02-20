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

#ifndef BRIGHTNESSPAGE_H_V20
#define BRIGHTNESSPAGE_H_V20

#include "interface/namespace.h"

#include <DTipLabel>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {
class TipsLabel;
class SwitchWidget;
class TitledSliderItem;
}

namespace display {
class Monitor;
class DisplayModel;
}

}

namespace DCC_NAMESPACE {

namespace display {

class BrightnessPage : public QWidget
{
    Q_OBJECT
public:
    explicit BrightnessPage(QWidget *parent = 0);

public:
    void setMode(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestSetMonitorBrightness(dcc::display::Monitor *, const double);
    void requestSetNightMode(bool);
    void requestAmbientLightAdjustBrightness(bool);

private:
    void addSlider();
    void removeSlider();
    QString brightnessToTickInterval(const double tb) const;

private Q_SLOT:
    void refreshSlider();

private:
    dcc::display::DisplayModel *m_displayModel{nullptr};

    QVBoxLayout *m_centralLayout{nullptr};
    DTK_WIDGET_NAMESPACE::DTipLabel *m_nightTips{nullptr};
    QList<dcc::widgets::TitledSliderItem *> m_sliders;

    dcc::widgets::SwitchWidget *m_nightShift{nullptr};
    dcc::widgets::SwitchWidget *m_autoLightMode{nullptr};
    int m_miniScales = 0;
};

}

}

#endif // BRIGHTNESSPAGE_H_V20
