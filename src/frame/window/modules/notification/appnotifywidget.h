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
class NotificationModel;
}
}

namespace DCC_NAMESPACE {
namespace notification {

class NotificationItem;

class AppNotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppNotifyWidget(int &index, dcc::notification::NotificationModel *model, QWidget *parent = nullptr);
    void setModel(dcc::notification::NotificationModel *model);

Q_SIGNALS:
    void requestSetAppSetting(const QString &appName, const QJsonObject &obj);

private:
    void initUI();
    void initConnect();

private:
    dcc::notification::NotificationModel *m_model;
    int m_index;//应用程序序号
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
