/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include <dtkwidget_global.h>

#include <QWidget>
#include <QVariant>

class QVBoxLayout;
class QLabel;

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DLabel;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {
class UserItemPic;
class OnlineIcon;

class UserItem : public QWidget
{
    Q_OBJECT
public:
    explicit UserItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setSelected(bool selected);
    void setOnline(bool online);
    void setPic(const QString &picPath);
    void setId(const QVariant &id);
    inline const QVariant id() const { return m_id; }

Q_SIGNALS:
    void selectedChanged(const bool selected) const;

private:
    QVBoxLayout *m_mainLayout;
    DTK_WIDGET_NAMESPACE::DLabel *m_title;
    UserItemPic *m_itemPic;  //picture of User
    QVariant m_id;
    OnlineIcon *m_onlineIcon;
};
}
