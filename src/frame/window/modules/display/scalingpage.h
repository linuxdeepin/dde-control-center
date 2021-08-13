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

#ifndef SCALINGPAGE_H_V20
#define SCALINGPAGE_H_V20

#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"

#include "interface/namespace.h"

#include <DTipLabel>

#include <QWidget>

#include <QGSettings/QGSettings>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class Resolution;

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
}
}

#define ALL_MONITOR_SAME_SCALE

namespace DCC_NAMESPACE {

namespace display {

class ScalingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ScalingPage(QWidget *parent = nullptr);

public:
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestUiScaleChange(const double scale) const;
    void requestIndividualScaling(dcc::display::Monitor *m, const double scale) const;

private Q_SLOT:
    void onResolutionChanged();

private:
    void setupSliders();
    void addSlider(int monitorID);

    QStringList getScaleList(const Resolution &r);
    int convertToSlider(const double value);
    double convertToScale(const int value);

    //获取当前分辨率下最佳缩放值
    double getBestScale();
private:
    dcc::display::DisplayModel *m_displayModel{nullptr};
    QVBoxLayout *m_centralLayout{nullptr};
    DTK_WIDGET_NAMESPACE::DTipLabel *m_tip{nullptr};
#ifdef ALL_MONITOR_SAME_SCALE
    dcc::widgets::TitledSliderItem *m_slider{nullptr};
#else
    QList<dcc::widgets::TitledSliderItem *> m_sliders;
#endif
};

}

}

#endif // SCALINGPAGE_H_V20
