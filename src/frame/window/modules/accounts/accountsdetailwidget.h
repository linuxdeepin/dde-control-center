// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/accounts/user.h"
#include "modules/accounts/avatarwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "avatarlistwidget.h"
#include "widgets/titlelabel.h"

#include <DLineEdit>
#include <DWarningButton>
#include <DToolButton>
#include <DSpinBox>

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DWarningButton;
class DCommandLinkButton;
class DLineEdit;
class DLabel;
class DSpinBox;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QLineEdit;
class QEvent;
class QCommandLinkButton;
class QStackedWidget;
class QGSettings;
QT_END_NAMESPACE

namespace dcc {
namespace accounts {
class UserModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountSpinBox : public DSpinBox
{
    Q_OBJECT

public:
    explicit AccountSpinBox(QWidget *parent = nullptr);
protected:
    virtual QString textFromValue(int val) const;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};


class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(dcc::accounts::User *user, dcc::accounts::UserModel *model, QWidget *parent = nullptr);
    virtual ~AccountsDetailWidget();
    void initHeadPart(QVBoxLayout *headLayout);
    void initBodyPart(QVBoxLayout *bodyLayout);
    void setAccountModel(dcc::accounts::UserModel *model);
    //获取其它用户是否开启自动登录开关
    QString getOtherUserAutoLogin();
    void setDeleteBtnStatus(const QString &key, const bool &status);
    void setModifyPwdBtnStatus(const QString &key);
    bool isSystemAdmin(dcc::accounts::User *user);

Q_SIGNALS:
    void requestShowPwdSettings(dcc::accounts::User *user);
    void requestShowSecurityQuestionsSettings(dcc::accounts::User *user);
    void requestSetAutoLogin(dcc::accounts::User *user, const bool autoLogin);
    void requestSetAdministrator(dcc::accounts::User *user, const bool asAdministrator);
    void requestNopasswdLogin(dcc::accounts::User *user, const bool nopasswdLogin);
    void requestDeleteAccount(dcc::accounts::User *user, const bool deleteHome);
    void requestBack();
    void requestSetAvatar(dcc::accounts::User *user, const QString &filePath);
    void requestSetFullname(dcc::accounts::User *user, const QString &fullname);
    void requestCleanThumbs(dcc::accounts::User *user);
    void requsetSetPassWordAge(dcc::accounts::User *user, const int age);
    void noticeEnrollCompleted(QString username);
    void editingFinished(const QString& userFullName);
    void requestSecurityQuestionsCheck(dcc::accounts::User *user);
    void requestShowUserGroups(dcc::accounts::User *user);

public Q_SLOTS:
    void resetDelButtonState();
    void onEditingFinished(bool isValid, const QString& userFullName);
    void onSecurityQuestionsCheckReplied(const QList<int> &questions);

protected:
    void initUserInfo(QVBoxLayout *layout);
    void initSetting(QVBoxLayout *layout);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    // 用户组
    void initUserGroup(QVBoxLayout *layout);

private Q_SLOTS:
    void deleteUserClicked();

private:
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;
    DLabel *m_fullName;//账户全名
    DTK_WIDGET_NAMESPACE::DToolButton *m_fullNameBtn;//账户全名编辑按钮
    DLineEdit *m_inputLineEdit;//账户全名编辑框
    AvatarListWidget *m_avatarListWidget;//图像列表
    bool m_isServerSystem;
    dcc::widgets::SwitchWidget *m_autoLogin;
    dcc::widgets::SwitchWidget *m_nopasswdLogin;
    dcc::widgets::SwitchWidget *m_asAdministrator;//作为管理员
    TitleLabel *m_accountSettingsTitle;//账户设置
    QHBoxLayout *m_avatarLayout;
    DWarningButton *m_deleteAccount;
    QPushButton *m_modifyPassword;
    QGSettings *m_gsettings;
    QScrollArea *m_scrollArea;
    QString m_groupName;
    dcc::accounts::User *m_curLoginUser;
    QLabel *m_bindStatusLabel;
};

}   // namespace accounts
}   // namespace dccV20
