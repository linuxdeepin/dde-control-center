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

#include <DLabel>
#include <DSwitchButton>

#include <QCheckBox>
#include <QVBoxLayout>
#include <QWidget>

namespace DCC_NAMESPACE {
class SwitchWidget;
class NormalLabel;
class SettingsGroup;

class AppItemModel;
class NotificationItem;

class AppNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppNotifyWidget(AppItemModel *model, QWidget *parent = nullptr);
    void setModel(AppItemModel *model);

Q_SIGNALS:
    void requestSetAppSetting(const QString &appName, uint item, QVariant var);

private:
    void initUI();
    void initConnect();

private:
    AppItemModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnAllowNotify; // 是否允许通知
    NotificationItem *m_itemNotifySound;          // 播放声音
    NotificationItem *m_itemLockShowNotify;       // 锁屏通知
    NotificationItem *m_itemShowInNotifyCenter;   // 仅通知中心显示
    NotificationItem *m_itemShowNotifyPreview;    // 显示预览
    Dtk::Widget::DLabel *m_lblTip;
    SettingsGroup *m_settingsGrp;
};
} // namespace DCC_NAMESPACE
