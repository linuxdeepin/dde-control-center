//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef AVATARLISVIEW_H
#define AVATARLISVIEW_H

#include "interface/namespace.h"

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

class AvatarListView : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT
public:
    enum ItemRole {
        AddAvatarRole = Dtk::UserRole + 1,
        SaveAvatarRole,
    };

public:
    AvatarListView(const int &role, const int &type, const QString &path, QWidget *parent = nullptr);
    virtual ~AvatarListView();

    inline int getCurrentListViewRole() { return m_currentAvatarRole; }
    inline int getCurrentListViewType() { return m_currentAvatarType; }
    inline QSize avatarSize() const { return m_avatarSize; }

    void addCustomAvatar(const QString &path, bool isFirst);
    void addLastItem();
    void setAvatarSize(const QSize &size);
    void saveAvatar(const QString &oldPath, const QString &path);
    void addItemFromDefaultDir(const QString &path);
    bool isExistCustomAvatar();

    QString getCustomAvatarPath();
    QString getAvatarPath() const;
    QString getCurrentSelectAvatar() const;

Q_SIGNALS:
    void requestUpdateListView(const int &role, const int &type);

public Q_SLOTS:
    void setCurrentAvatarChecked(const QString &avatar);
    void setCurrentAvatarUnChecked();
    void requestUpdateCustomAvatar(const QString &path);

private:
    void initWidgets();
    QStandardItem *getCustomAvatar();

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);

private:
    bool m_updateItem = false;
    int m_currentAvatarRole;
    int m_currentAvatarType;
    QString m_path;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
    QSize m_avatarSize;
    QModelIndex m_currentSelectIndex;
    QFileDialog *m_fd;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
};
}

#endif //AVATARLISVIEW_H