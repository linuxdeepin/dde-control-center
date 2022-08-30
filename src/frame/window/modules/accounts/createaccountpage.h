// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <DComboBox>
#include "interface/namespace.h"
#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/creationresult.h"
#include "avatarlistwidget.h"
#include "widgets/lineeditwidget.h"
#include "accountswidget.h"
#include "modules/accounts/accountsworker.h"

#include <DLineEdit>
#include <DSuggestButton>
#include <DPasswordEdit>

#include <QWidget>
#include <QScrollArea>

#include "com_deepin_defender_hmiscreen.h"
#include "com_deepin_defender_daemonservice.h"

using Defender = com::deepin::defender::hmiscreen;
using DaemonService = com::deepin::defender::daemonservice;

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
DWIDGET_USE_NAMESPACE
#define NAME_ALREADY 4

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

const int PwdLimitLowestLevel = 1;

//创建账户页面
class CreateAccountPage : public QWidget
{
    Q_OBJECT

public:
    enum PassWordType {
        NormalPassWord,
        IncludeBlankSymbol
    };

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
    explicit CreateAccountPage(dcc::accounts::AccountsWorker *accountsWorker, QWidget *parent = nullptr);
    ~CreateAccountPage();
    void setModel(dcc::accounts::UserModel *userModel, dcc::accounts::User *user);

private:
    void initWidgets(QVBoxLayout *layout);
    void initUsrGroup(QVBoxLayout *layout);
    void createUser();
    void showGroupList(const QString &index);

protected:
    void showEvent(QShowEvent *event) override;

Q_SIGNALS:
    void requestCreateUser(const dcc::accounts::User *user);
    void requestBack(DCC_NAMESPACE::accounts::AccountsWidget::ActionOption option = DCC_NAMESPACE::accounts::AccountsWidget::ClickCancel);
    void requestSetPasswordHint(dcc::accounts::User *, const QString &);
    void requestCheckPwdLimitLevel();

public Q_SLOTS:
    void setCreationResult(dcc::accounts::CreationResult *result);

protected:
    void resizeEvent(QResizeEvent *e) override;

private Q_SLOTS:
    bool checkName();
    bool checkFullname(bool showTips = true);
    bool checkPassword(DPasswordEdit *edit, bool &needShowSafetyPage, bool showTips = true);

private:
    dcc::accounts::User *m_newUser;
    dcc::accounts::UserModel *m_userModel;
    dcc::accounts::AccountsWorker *m_accountWorker;
    AvatarListWidget *m_avatarListWidget;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_nameEdit;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_fullnameEdit;
    DPasswordEdit *m_passwdEdit;
    DPasswordEdit *m_repeatpasswdEdit;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_passwdTipsEdit;
    DTK_WIDGET_NAMESPACE::DComboBox *m_accountChooser;

    DTK_WIDGET_NAMESPACE::DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
    bool m_isServerSystem;
    QWidget *m_tw;
    QScrollArea *m_scrollArea;
    QLabel *m_groupTip;
};

}
}
