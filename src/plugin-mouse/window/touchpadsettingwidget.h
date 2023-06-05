//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHPADSETTINGWIDGET_H
#define TOUCHPADSETTINGWIDGET_H

#include "interface/namespace.h"
#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class MouseModel;
class MouseWorker;
class DouTestWidget;
class PalmDetectSetting;

class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}

namespace DCC_NAMESPACE {
class TouchpadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TouchpadSettingWidget(QWidget *parent = nullptr);
    void setModel(MouseModel *const model);

Q_SIGNALS:
    void requestSetTouchpadMotionAcceleration(const int value);
    void requestSetTouchpadEnabled(const bool enabled);
    void requestSetTapClick(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);

private Q_SLOTS:
    void onTouchMoveSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    PalmDetectSetting *m_palmDetectSetting;
    TitledSliderItem *m_touchMoveSlider;
    SwitchWidget *m_touchpadEnableBtn;
    SwitchWidget *m_touchClickStn;
    SwitchWidget *m_touchNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
#endif // TOUCHPADSETTINGWIDGET_H
