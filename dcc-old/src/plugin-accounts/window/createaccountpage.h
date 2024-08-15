// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "avatarlistwidget.h"
#include "interface/namespace.h"
#include "src/plugin-accounts/operation/accountsworker.h"
#include "src/plugin-accounts/operation/creationresult.h"
#include "src/plugin-accounts/operation/user.h"
#include "src/plugin-accounts/operation/usermodel.h"
#include "widgets/lineeditwidget.h"

#include <DAbstractDialog>
#include <DComboBox>
#include <DLineEdit>
#include <DPasswordEdit>
#include <DSuggestButton>

#include <QDialog>

#define NAME_ALREADY 4

DWIDGET_USE_NAMESPACE

class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;

namespace DCC_NAMESPACE {
class ComboxWidget;
class SecurityLevelItem;

const int PwdLimitLowestLevel = 1;

// 创建账户页面
class CreateAccountPage : public DAbstractDialog
{
    Q_OBJECT

public:
    enum PassWordType { NormalPassWord, IncludeBlankSymbol };

    enum PassErrorCode {
        ErrCodeEmpty = 1,
        ErrCodeInvalidChar,
        ErrCodeFirstCharInvalid,
        ErrCodeExist,
        ErrCodeNameExist,
        ErrCodeSystemUsed,
        ErrCodeLen
    };

public:
    explicit CreateAccountPage(AccountsWorker *accountsWorker, QWidget *parent = nullptr);
    ~CreateAccountPage();
    void setModel(UserModel *userModel, User *user);

private:
    void initWidgets(QVBoxLayout *layout);
    void initUsrGroup(QVBoxLayout *layout);
    void createUser();
    void showGroupList(const QString &index);

protected:
    void showEvent(QShowEvent *event) override;

Q_SIGNALS:
    void requestCreateUser(const User *user);
    void requestSetPasswordHint(User *, const QString &);
    void requestCheckPwdLimitLevel();

public Q_SLOTS:
    void setCreationResult(CreationResult *result);

protected:
    void resizeEvent(QResizeEvent *e) override;

private Q_SLOTS:
    bool checkName();
    bool checkFullname();
    bool checkPassword(DPasswordEdit *edit, bool &needShowSafetyPage);

private:
    User *m_newUser;
    UserModel *m_userModel;
    AccountsWorker *m_accountWorker;
    LineEditWidget *m_nameEdit;
    LineEditWidget *m_fullnameEdit;
    DPasswordEdit *m_passwdEdit;
    DPasswordEdit *m_repeatpasswdEdit;
    Dtk::Widget::DLineEdit *m_passwdTipsEdit;
    ComboxWidget *m_accountChooser;
    DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
    bool m_isServerSystem;
    QWidget *m_tw;
    QScrollArea *m_scrollArea;
    QLabel *m_groupTip;
    SecurityLevelItem *m_securityLevelItem;
};
} // namespace DCC_NAMESPACE
