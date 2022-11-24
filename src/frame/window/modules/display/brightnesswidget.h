// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BRIGHTNESSWIDGET_H
#define BRIGHTNESSWIDGET_H

#include "interface/namespace.h"
#include "widgets/titlelabel.h"

#include <DTipLabel>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QSpacerItem;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {
class SwitchWidget;
class TitledSliderItem;
} // namespace widgets

namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class BrightnessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BrightnessWidget(QWidget *parent = 0);
    ~BrightnessWidget();

public:
    void setMode(dcc::display::DisplayModel *model);
    void showBrightness(dcc::display::Monitor *monitor = nullptr);

Q_SIGNALS:
    void requestSetMonitorBrightness(dcc::display::Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);

private:
    void addSlider();
    QString brightnessToTickInterval(const double tb) const;

private:
    dcc::display::DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
    TitleLabel *m_brightnessTitle;
    QSpacerItem *m_autoLightSpacerItem;
    dcc::widgets::SwitchWidget *m_autoLightMode;

    int m_miniScales = 0;
    QMap<dcc::display::Monitor *, QWidget *> m_monitorBrightnessMap;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // BRIGHTNESSWIDGET_H
