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

#include "../../namespace.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QList>


#include "widgets/titledslideritem.h"

class QLabel;

namespace dcc {

namespace widgets {
class TipsLabel;
class SwitchWidget;
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

    void setMode(dcc::display::DisplayModel *model);
Q_SIGNALS:
    void requestSetMonitorBrightness(dcc::display::Monitor *, const double);

    void requestSetNightMode(bool);
    void requestAmbientLightAdjustBrightness(bool);

private:
    void addSlider();
private:
    dcc::display::DisplayModel *m_displayModel{nullptr};

    QVBoxLayout *m_centralLayout{nullptr};
    QList<dcc::widgets::TitledSliderItem *> m_sliders;


    QLabel *m_nightTips{nullptr};

    dcc::widgets::SwitchWidget *m_nightShift{nullptr};
    dcc::widgets::SwitchWidget *m_autoLightMode{nullptr};
};

}

}

#endif // BRIGHTNESSPAGE_H_V20
