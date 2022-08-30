// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <DSwitchButton>
#include <DLabel>

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>

namespace dcc {
namespace widgets {
class SwitchWidget;
class NormalLabel;
class SettingsGroup;
}

namespace notification {
class AppItemModel;
}
}

namespace DCC_NAMESPACE {
namespace notification {

class NotificationItem;

class AppNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppNotifyWidget(dcc::notification::AppItemModel *model, QWidget *parent = nullptr);
    void setModel(dcc::notification::AppItemModel *model);

Q_SIGNALS:
    void requestSetAppSetting(const QString &appName, uint item, QVariant var);

private:
    void initUI();
    void initConnect();

private:
    dcc::notification::AppItemModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnAllowNotify;//是否允许通知
    NotificationItem *m_itemNotifySound;//播放声音
    NotificationItem *m_itemLockShowNotify;//锁屏通知
    NotificationItem *m_itemShowInNotifyCenter;//仅通知中心显示
    NotificationItem *m_itemShowNotifyPreview;//显示预览
    Dtk::Widget::DLabel *m_lblTip;
    dcc::widgets::SettingsGroup *m_settingsGrp;
};

}
}
