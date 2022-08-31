// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace notification {

//消息通知选择项
class NotificationItem: public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit NotificationItem(QWidget *parent = nullptr);
    void setTitle(const QString &title);

    bool getState() const;
    void setState(const bool &state);
Q_SIGNALS:
    void stateChanged(bool state);

private:
    QHBoxLayout *m_layout;
    QCheckBox *m_chkState;
};

}// namespace dccV20
}// namespace msgnotify
