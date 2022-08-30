// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <DLineEdit>

#include <QWidget>
#include <QVBoxLayout>

namespace dcc {
namespace widgets {
class SwitchWidget;
class NormalLabel;
class SettingsGroup;
}

namespace notification {
class SysItemModel;
}
}

DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
class DSwitchButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace notification {

class NotificationItem;
class TimeSlotItem;

class SystemNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SystemNotifyWidget(dcc::notification::SysItemModel *model, QWidget *parent = nullptr);
    void setModel(dcc::notification::SysItemModel *model);

Q_SIGNALS:
    void requestSetSysSetting(uint item, QVariant var);

private:
    void initUI();
    void initConnect();

private:
    dcc::notification::SysItemModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnDisturbMode;//勿扰模式
    TimeSlotItem *m_itemTimeSlot;//时间段
    NotificationItem *m_itemLockScreen;
    dcc::widgets::SettingsGroup *m_settingsGrp;//自选项
    Dtk::Widget::DLineEdit *m_editTimeStart;//时间段开始
    Dtk::Widget::DLineEdit *m_editTimeEnd;//时间段结束
};

}   //namespace notification
}   //namesapce dccV20
