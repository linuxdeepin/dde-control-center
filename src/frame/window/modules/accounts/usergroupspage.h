// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "accountswidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsitem.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>
#include <DLineEdit>

#include <QLabel>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

class UserGroupsInfoItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit UserGroupsInfoItem(QWidget *parent = nullptr);
    virtual ~UserGroupsInfoItem();
    void setTitle(const QString &title);
    QString getTitle() const { return m_itemName; };
    void alertTitleRepeat();
    void appendItem(QWidget *widget);
    void setShowIcon(bool state);
    void setEditTitle(bool state);
    void setHideTitle(bool state);
    bool onNameEditFinished();

    void setEditStatus(bool status);
    void setCanEdit(bool canEdit);

    void showAlertMessage(const QString &errMsg);
    void setInGroup(bool inGroup) {
        m_isInGroup = inGroup;
        if (!m_editStatus) {
            if (m_isInGroup)
                m_checkedBtn->setVisible(true);
            else
                m_checkedBtn->setVisible(false);
        }
    }
    const bool isInGroup() { return m_isInGroup; }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;


Q_SIGNALS:
    void removeClicked(UserGroupsInfoItem *item, QString group);
    void editClicked(bool state);
    void editTextFinished(UserGroupsInfoItem *item, QString group);
    void itemClicked(UserGroupsInfoItem *item, QString group);
    void cancelAddGroup();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QHBoxLayout *m_layout;
    QLabel *m_title;
    DIconButton *m_removeBtn;
    DIconButton *m_checkedBtn;

    DIconButton *m_editBtn;
    DLineEdit *m_editTitle;
    QString m_itemName;
    DPalette m_currentpa;
    bool m_isInGroup;
    bool m_editStatus;
    bool m_canEdit;
};

class UserGroupsPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit UserGroupsPage(dcc::accounts::User *user, dcc::accounts::UserModel *userModel, dcc::ContentWidget *parent = nullptr);
    ~UserGroupsPage();

Q_SIGNALS:
    void requestSetGroups(dcc::accounts::User *user, const QStringList &usrGroups);
    void requestDeleteGroup(const QString &group);
    void requestCreateGroup(const QString &group, uint32_t gid, bool isSystem);
    void requestModifyGroup(const QString &oldGroup, const QString &newGroup, uint32_t gid);
    void requestGetGroupInfo(const QString &groupName, QString &resInfoJson);

private Q_SLOTS:
    void changeUserGroup(const QStringList &groups);
    void GroupClicked(UserGroupsInfoItem *item, const QString &groupName);
    void setGroupInfo(const QStringList &group);
    void onGidChanged(const QString &gid);
    void cancelAddGroup();
    void editTextFinished(UserGroupsInfoItem *item, QString group);
    void removeClicked(UserGroupsInfoItem *item, QString group);

    void addGroupItem();
    void onItemEdit();

public Q_SLOTS:
    void onGroupListChanged(const QStringList & value);

private:
    void initWidget();
    void initData();
    int getAdministratorAccountsCount();

private:
    QString m_groupName;
    QLabel *m_groupTip;
    QVBoxLayout * m_layout;
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;
    DTK_WIDGET_NAMESPACE::DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;

    dcc::widgets::SettingsGroup *m_listGrp;
    QVector<UserGroupsInfoItem*> m_vecItem;
    DCommandLinkButton *m_editBtn;
    DCommandLinkButton *m_addBtn;
    bool m_isItemEditting;
    bool m_isAddItem;
};
}
}
