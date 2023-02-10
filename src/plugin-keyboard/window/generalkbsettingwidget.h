//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GENERALKBSETTINGWIDGET_H
#define GENERALKBSETTINGWIDGET_H

#include "interface/namespace.h"
#include <QMouseEvent>
#include <QWidget>

#include <DLineEdit>
#include <DPaletteHelper>
#include <DStyle>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class KeyboardModel;

class SettingsGroup;
class SwitchWidget;
class DCCSlider;
class GLineEdit;

class GeneralKBSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralKBSettingWidget(KeyboardModel *model, QWidget *parent = nullptr);
Q_SIGNALS:
    void requestKBDelayChanged(const int value);
    void requestKBSpeedChanged(const int value);
    void requestNumLockChanged(const bool state);
    void requestCapsLockChanged(const bool state);
private Q_SLOTS:
    void setDelayValue(uint value);
    void setSpeedValue(uint value);
private:
    DCCSlider *m_delaySlider;
    DCCSlider *m_speedSlider;
    SwitchWidget *m_upper;
    SwitchWidget *m_numLock;
    SettingsGroup *m_generalSettingsGrp;
    KeyboardModel *m_model;
    QVBoxLayout *m_contentLayout;
    DLineEdit *m_testArea;
};
}
#endif // GENERALKBSETTINGWIDGET_H
