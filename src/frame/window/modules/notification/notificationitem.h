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
