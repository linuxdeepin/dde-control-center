/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
class NotificationModel;
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
    explicit SystemNotifyWidget(dcc::notification::NotificationModel *model, QWidget *parent = nullptr);
    void setModel(dcc::notification::NotificationModel *model);

Q_SIGNALS:
    void requestSetSysSetting(const QJsonObject &obj);

private:
    void initUI();
    void initConnect();

private:
    dcc::notification::NotificationModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnDisturbMode;//勿扰模式
    dcc::widgets::SwitchWidget *m_btnShowInDock;//是否显示在Dock
    TimeSlotItem *m_itemTimeSlot;//时间段
    NotificationItem *m_itemFullScreen;
    NotificationItem *m_itemProjector;
    NotificationItem *m_itemLockScreen;
    dcc::widgets::SettingsGroup *m_settingsGrp;//自选项
    Dtk::Widget::DLineEdit *m_editTimeStart;//时间段开始
    Dtk::Widget::DLineEdit *m_editTimeEnd;//时间段结束
};

}   //namespace notification
}   //namesapce dccV20
