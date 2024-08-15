//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MOUSESETTINGWIDGET_H
#define MOUSESETTINGWIDGET_H

#include "interface/namespace.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"

#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
class MouseSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MouseSettingWidget(QWidget *parent = nullptr);
    ~MouseSettingWidget();

    void setModel(MouseModel *const model);

Q_SIGNALS:
    void requestSetMouseMotionAcceleration(const int value);
    void requestSetAccelProfile(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetMouseNaturalScroll(const bool state);

private Q_SLOTS:
    void onMouseMoveSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    SettingsGroup *m_mouseSettingsGrp;
    TitledSliderItem *m_mouseMoveSlider;
    SwitchWidget *m_adaptiveAccelProfile;
    SwitchWidget *m_disTchStn;
    SwitchWidget *m_mouseNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
#endif // MOUSESETTINGWIDGET_H
