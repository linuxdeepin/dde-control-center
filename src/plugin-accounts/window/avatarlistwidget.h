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

#include "interface/namespace.h"
#include "src/plugin-accounts/operation/user.h"

#include <DDialog>
#include <DListView>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
class QListView;
class QStandardItemModel;
class QModelIndex;
class QFileDialog;
QT_END_NAMESPACE

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

namespace DCC_NAMESPACE {
class AvatarItemDelegate;

class AvatarListWidget : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT
public:
    enum ItemRole {
        AddAvatarRole = Dtk::UserRole + 1,
        SaveAvatarRole,
    };

public:
    AvatarListWidget(User *usr, QWidget *parent = nullptr);
    virtual ~AvatarListWidget();

public:
    void addItemFromDefaultDir();
    void addLastItem();
    QString getAvatarPath() const;

    inline QSize avatarSize() const { return m_avatarSize; }

    void setAvatarSize(const QSize &size);

Q_SIGNALS:
    void requestSetAvatar(const QString &avatarPath);
    void requestAddNewAvatar(User *user, const QString &file);
    void requesRetract();

public Q_SLOTS:
    void setCurrentAvatarChecked(const QString &avatar);
    void refreshCustomAvatar(const QString &str);

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);

private:
    void initWidgets();
    QString getUserAddedCustomPicPath(const QString &usrName);
    QStandardItem *getCustomAvatar();

private:
    User *m_curUser{ nullptr };
    QVBoxLayout *m_mainContentLayout;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
    QSize m_avatarSize;
    QModelIndex m_currentSelectIndex;
    bool m_displayLastItem;
    QFileDialog *m_fd;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
};

class AvatarListDialog : public DTK_WIDGET_NAMESPACE::DDialog
{
    Q_OBJECT
public:
    AvatarListDialog(User *usr, QWidget *parent = nullptr);
    virtual ~AvatarListDialog();

    QString getAvatarPath() const;

private:
    AvatarListWidget *m_avatarList;
};
} // namespace DCC_NAMESPACE
