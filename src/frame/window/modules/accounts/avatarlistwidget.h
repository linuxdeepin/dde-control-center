// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
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
class QFileDialog;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {
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
    AvatarListWidget(dcc::accounts::User *usr, QWidget *parent = nullptr);
    virtual ~AvatarListWidget();
public:
    void addItemFromDefaultDir();
    void addLastItem();
    QString getAvatarPath() const;
    inline QSize avatarSize() const { return m_avatarSize; }
    void setAvatarSize(const QSize &size);

Q_SIGNALS:
    void requestSetAvatar(const QString &avatarPath);
    void requestAddNewAvatar(dcc::accounts::User *user, const QString &file);
    void requesRetract();

public Q_SLOTS:
    void setCurrentAvatarChecked(const QString &avatar);
    void refreshCustomAvatar(const QString &str);

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);

private:
    void initWidgets();
    QString getUserAddedCustomPicPath(const QString &usrName);

private:
    dcc::accounts::User *m_curUser{nullptr};
    QVBoxLayout *m_mainContentLayout;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
    QSize m_avatarSize;
    QModelIndex m_currentSelectIndex;
    bool m_displayLastItem;
    QFileDialog *m_fd;
};

}
}
