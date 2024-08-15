//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <DLineEdit>

#include <QWidget>
#include <QVBoxLayout>

DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
class DSwitchButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SwitchWidget;
class NormalLabel;
class SettingsGroup;

class SysItemModel;

class NotificationItem;
class TimeSlotItem;

class SystemNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SystemNotifyWidget(SysItemModel *model, QWidget *parent = nullptr);
    void setModel(SysItemModel *model);

Q_SIGNALS:
    void requestSetSysSetting(uint item, QVariant var);

private:
    void initUI();
    void initConnect();

private:
    SysItemModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnDisturbMode;//勿扰模式
    TimeSlotItem *m_itemTimeSlot;//时间段
    NotificationItem *m_itemLockScreen;
    SettingsGroup *m_settingsGrp;//自选项
    Dtk::Widget::DLineEdit *m_editTimeStart;//时间段开始
    Dtk::Widget::DLineEdit *m_editTimeEnd;//时间段结束
};
}
