//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

//消息通知选择项
class NotificationItem: public SettingsItem
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

}
