//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GENERALSETTINGWIDGET_H
#define GENERALSETTINGWIDGET_H

#include "interface/namespace.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"

#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class DouTestWidget;

class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}

namespace DCC_NAMESPACE {
class GeneralSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralSettingWidget(QWidget *parent = nullptr);
    ~GeneralSettingWidget();

    void setModel(MouseModel *const model);
Q_SIGNALS:
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestScrollSpeed(const int speed);
    void requestSetDouClick(const int value);
private:
    void onDoubleClickSpeedChanged(int speed);
    void onScrollSpeedChanged(uint speed);
private:
    MouseModel *m_mouseModel;
    SettingsGroup *m_generalSettingsGrp;

    SwitchWidget *m_leftHand;
    SwitchWidget *m_disInTyping;
    TitledSliderItem *m_doubleSlider;
    TitledSliderItem *m_scrollSpeedSlider;
    DouTestWidget *m_doubleTest;
    QVBoxLayout *m_contentLayout;
};
}
#endif // GENERALSETTINGWIDGET_H
