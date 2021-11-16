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

#include <DComboBox>
#include "interface/namespace.h"
#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/creationresult.h"
#include "avatarlistwidget.h"
#include "widgets/lineeditwidget.h"
#include "accountswidget.h"
#include "modules/accounts/accountsworker.h"

#include <ddialog.h>
#include <DLineEdit>
#include <dpasswordedit.h>
#include <DSuggestButton>

#include <QWidget>
#include <QScrollArea>

#include "com_deepin_defender_hmiscreen.h"
#include "com_deepin_defender_daemonservice.h"

using Defender = com::deepin::defender::hmiscreen;
using DaemonService = com::deepin::defender::daemonservice;

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
#define NAME_ALREADY 4

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

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

public Q_SLOTS:
    void setCreationResult(dcc::accounts::CreationResult *result);

protected:
    void resizeEvent(QResizeEvent *e) override;

private Q_SLOTS:
    bool checkName();
    bool checkFullname();
    bool checkPassword(DPasswordEdit *edit);

private:
    dcc::accounts::User *m_newUser;
    dcc::accounts::UserModel *m_userModel;
    dcc::accounts::AccountsWorker *m_accountWorker;
    AvatarListWidget *m_avatarListWidget;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_nameEdit;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_fullnameEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_passwdEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_repeatpasswdEdit;
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
