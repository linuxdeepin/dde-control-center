//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <DSwitchButton>
#include <DLabel>

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>

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
    Dtk::Widget::DSwitchButton *m_btnAllowNotify;//是否允许通知
    NotificationItem *m_itemNotifySound;//播放声音
    NotificationItem *m_itemLockShowNotify;//锁屏通知
    NotificationItem *m_itemShowInNotifyCenter;//仅通知中心显示
    NotificationItem *m_itemShowNotifyPreview;//显示预览
    Dtk::Widget::DLabel *m_lblTip;
    SettingsGroup *m_settingsGrp;
};
}
