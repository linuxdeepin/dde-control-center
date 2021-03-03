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

#ifndef SCALINGWIDGET_H
#define SCALINGWIDGET_H

#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/titlelabel.h"
#include "interface/namespace.h"

#include <DTipLabel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class Resolution;

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
} // namespace display
} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class ScalingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScalingWidget(QWidget *parent = nullptr);
    ~ScalingWidget();

public:
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestUiScaleChange(const double scale);

private Q_SLOTS:
    void onResolutionChanged();

private:
    void addSlider();
    QStringList getScaleList(const Resolution &r);
    int convertToSlider(const double value);

private:
    dcc::display::DisplayModel *m_displayModel;
    QVBoxLayout *m_centralLayout;
    TitleLabel *m_title;
    QWidget *m_tipWidget;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_tipLabel;
    dcc::widgets::TitledSliderItem *m_slider;
    QStringList m_scaleList;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // SCALINGWIDGET_H
