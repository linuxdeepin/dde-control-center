// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef AVATARLISVIEW_H
#define AVATARLISVIEW_H

#include "interface/namespace.h"
#include "src/plugin-accounts/operation/user.h"

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

enum Role { Person, Animal, Scenery, Illustration, Expression, Custom, AvatarAdd };

enum Type {
    Cartoon,     // 卡通风格
    Dimensional, // 立体风格
    Flat         // 平面风格
};

class AvatarListView : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT
public:
    enum ItemRole {
        AddAvatarRole = Dtk::UserRole + 1,
        SaveAvatarRole,
    };

public:
    AvatarListView(User *user, const int &role,
                   const int &type,
                   const QString &path,
                   QWidget *parent = nullptr);
    virtual ~AvatarListView();

    inline int getCurrentListViewRole() const { return m_currentAvatarRole; }
    inline int getCurrentListViewType() const { return m_currentAvatarType; }
    inline QSize avatarSize() const { return m_avatarSize; }

    void addCustomAvatar(const QString &path);
    void addLastItem();
    void saveAvatar(const QString &path);
    void addItemFromDefaultDir(const QString &path);

    QString getAvatarPath() const;

Q_SIGNALS:
    void requestUpdateListView(bool isSave, const int &role, const int &type);
    void requestDeleteUserIcon(const QString &iconPath);

public Q_SLOTS:
    void setCurrentAvatarChecked(const QString &avatar);
    void setCurrentAvatarUnChecked();
    void requestAddCustomAvatar(const QString &path);
    void requestUpdateCustomAvatar(const QString &path);

protected:
    void updateGeometries() override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void initWidgets();
    QStandardItem *getCustomAvatar();
    bool checkIsToDeleteAvatar(const QModelIndex &index);

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);

private:
    bool m_updateItem = false;
    bool m_save = false;
    int m_currentAvatarRole;
    int m_currentAvatarType;
    QString m_path;
    QStandardItemModel *m_avatarItemModel;
    AvatarItemDelegate *m_avatarItemDelegate;
    QSize m_avatarSize;
    QModelIndex m_currentSelectIndex;
    User *m_curUser;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
};
} // namespace DCC_NAMESPACE

#endif // AVATARLISVIEW_H
