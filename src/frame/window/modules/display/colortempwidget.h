// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COLORTEMPWIDGET_H
#define COLORTEMPWIDGET_H

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
class TipsLabel;
class SwitchWidget;
class SettingsGroup;
class TitledSliderItem;
} // namespace widgets

namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

namespace DCC_NAMESPACE {
namespace display {

class ColorTempWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorTempWidget(QWidget *parent = nullptr);
    ~ColorTempWidget();

public:
    void setMode(dcc::display::DisplayModel *model);
    void setAdjustCCTmode(int mode);

Q_SIGNALS:
    void requestSetColorTemperature(const int value);
    void requestSetMethodAdjustCCT(const int mode);

private:
    void addSlider();
    int colorTemperatureToValue(int kelvin);
    void setColorTemperatureVisible(bool visible);

private:
    dcc::display::DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
    QWidget *m_tempratureColorWidget;
    TitleLabel *m_tempratureColorTitle;
    dcc::widgets::SwitchWidget *m_nightShift;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_nightTips;
    dcc::widgets::SettingsGroup *m_settingsGroup;
    dcc::widgets::SwitchWidget *m_nightManual;
    dcc::widgets::TitledSliderItem *m_cctItem;
    QSpacerItem *m_nightShiftSpacerItem;
    QSpacerItem *m_nightTipsSpacerItem;
    QSpacerItem *m_nightManualSpacerItem;
};

} // namespace display
} // namespace DCC_NAMESPACE

#endif // COLORTEMPWIDGET_H
