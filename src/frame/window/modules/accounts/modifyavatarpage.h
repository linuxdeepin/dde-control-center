/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "window/namespace.h"
#include <QWidget>

class QVBoxLayout;
class QLabel;
class QListView;
class QStandardItemModel;

namespace dcc {
namespace accounts {
class User;
class AvatarWidget;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AvatarItemDelegate;
class ModifyAvatarPage : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyAvatarPage(dcc::accounts::User *user, QWidget *parent = nullptr);
    void initWidgets();
    void initDatas();

private:
    dcc::accounts::User *m_curUser;//当前用户
    QVBoxLayout *m_mainContentLayout;
    QVBoxLayout *m_headLayout;
    QVBoxLayout *m_avatarlistLayout;
    dcc::accounts::AvatarWidget *m_avatar;//账户图片
    QLabel *m_shortName;
    QLabel *m_fullName;
    QListView *m_avatarListView;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
};

}
}
