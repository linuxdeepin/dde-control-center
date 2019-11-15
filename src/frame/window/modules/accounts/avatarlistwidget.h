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
#include "modules/accounts/user.h"

#include <DStyle>
#include <DStyleOption>
#include <DIconButton>
#include <DListView>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
class QListView;
class QStandardItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {
class AvatarItemDelegate;
class AvatarListWidget : public QWidget
{
    Q_OBJECT
public:
    AvatarListWidget(QWidget *parent = nullptr, bool displayLastItem = true);
    void initWidgets();
    void initDatas();
    void setUserModel(dcc::accounts::User *user);
    void setCurrentAvatarChecked(const QString &avatar);

    enum ItemRole {
        AddAvatarRole = Dtk::UserRole + 1,
        SaveAvatarRole,
    };

    void addItemFromDefaultDir();
    void addLastItem();
    void addSecond2Last(QString newiconpath);
    QString getUserAddedCustomPicPath();
    bool checkFileIsExists(QString path);
    QString getAvatarPath(int n) const;
    int getCurrentSelectIndex() const;

Q_SIGNALS:
    void requestSetAvatar(const QString &avatarPath);
    void requestAddNewAvatar(dcc::accounts::User *user);
    void requestDeleteAvatar(const QString &iconPath);

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void onAddNewAvatarSuccess(bool added);
    void onSetAvatarSuccess(bool modified);

private:
    dcc::accounts::User *m_curUser;
    QVBoxLayout *m_mainContentLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_avatarListView;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
    QList<QString> m_iconpathList;
    int m_prevSelectIndex;
    int m_currentSelectIndex;
    bool m_displayLastItem;
};

}
}
