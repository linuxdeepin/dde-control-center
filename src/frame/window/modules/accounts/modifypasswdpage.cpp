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

#include "modifypasswdpage.h"
#include "widgets/titlelabel.h"
#include "pwqualitymanager.h"
#include "../../utils.h"
#include "createaccountpage.h"
#include "widgets/securitylevelitem.h"

#include "deepin_pw_check.h"
#include "unionidbindreminderdialog.h"
#include <DDialog>
#include <DDBusSender>
#include <DDesktopServices>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QDebug>

#include <unistd.h>

using namespace dcc::accounts;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

ModifyPasswdPage::ModifyPasswdPage(User *user, bool isCurrent, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_isCurrent(isCurrent)
    , m_isBindCheckError(false)
    , m_securityLevelItem(new SecurityLevelItem(this))

{
    initWidget();
}

ModifyPasswdPage::~ModifyPasswdPage()
{

}

void ModifyPasswdPage::initWidget()
{
    this->setAccessibleName("ModifyPasswdPage");
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addSpacing(40);
    mainContentLayout->setSpacing(4);
    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    if (!m_isCurrent) {
        titleLabel->setText(tr("Reset Password"));
        QLabel *label = new QLabel(tr("Resetting the password will clear the data stored in the keyring."));
        label->setWordWrap(true);
        mainContentLayout->addWidget(label, 0, Qt::AlignHCenter);
    }
    mainContentLayout->addSpacing(40);

    if (m_isCurrent) {
        QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
        m_forgetPasswordBtn = new DCommandLinkButton(tr("Forgot password?"));
        DFontSizeManager::instance()->bind(m_forgetPasswordBtn, DFontSizeManager::T8);
        m_forgetPasswordBtn->setVisible(!IsCommunitySystem && getuid() < 9999); // 如果当前账户是域账号,则屏蔽重置密码入口
        connect(m_forgetPasswordBtn, &QPushButton::clicked, this, &ModifyPasswdPage::onForgetPasswordBtnClicked);
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(oldPasswdLabel);
        hLayout->addStretch();
        hLayout->addWidget(m_forgetPasswordBtn);
        hLayout->addSpacing(45);
        mainContentLayout->addLayout(hLayout);
        mainContentLayout->addWidget(m_oldPasswordEdit);
    }

    QHBoxLayout *newPasswdLayout = new QHBoxLayout;
    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    newPasswdLayout->addWidget(newPasswdLabel);
    newPasswdLayout->addSpacing(80);

    newPasswdLayout->addWidget(m_securityLevelItem);
    mainContentLayout->addSpacing(6);
    mainContentLayout->addLayout(newPasswdLayout);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(passwdTipsLabel);
    mainContentLayout->addWidget(m_passwordTipsEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    mainContentLayout->addLayout(cansaveLayout);
    setLayout(mainContentLayout);
    cancleBtn->setDefault(true);
    saveBtn->setDefault(true);
    cancleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setPasswordEditAttribute(m_oldPasswordEdit);
    setPasswordEditAttribute(m_newPasswordEdit);
    setPasswordEditAttribute(m_repeatPasswordEdit);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });
    connect(m_curUser, &User::passwordResetFinished, this, &ModifyPasswdPage::resetPasswordFinished);
    connect(m_curUser, &User::startResetPasswordReplied, this, &ModifyPasswdPage::onStartResetPasswordReplied);

    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert())
        {
            m_oldPasswordEdit->hideAlertMessage();
            m_oldPasswordEdit->setAlert(false);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert())
        {
            m_repeatPasswordEdit->hideAlertMessage();
            m_repeatPasswordEdit->setAlert(false);
        }
    });
    connect(m_passwordTipsEdit, &DLineEdit::textEdited, this, [ = ](const QString & passwdTips) {
        if (passwdTips.size() > 14) {
            m_passwordTipsEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_passwordTipsEdit->isAlert()) {
            m_passwordTipsEdit->setAlert(false);
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::textChanged, this, [ = ]() {
        if (m_newPasswordEdit->text().isEmpty()) {
            m_securityLevelItem->setLevel(SecurityLevelItem::NoneLevel);
            m_newPasswordEdit->setAlert(false);
            m_newPasswordEdit->hideAlertMessage();
            return ;
        }
        PASSWORD_LEVEL_TYPE m_level = PwqualityManager::instance()->GetNewPassWdLevel(m_newPasswordEdit->text());
        PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_newPasswordEdit->lineEdit()->text(),
                                                                                          m_newPasswordEdit->lineEdit()->text());

        if (m_level == PASSWORD_STRENGTH_LEVEL_HIGH) {
            m_securityLevelItem->setLevel(SecurityLevelItem::HighLevel);
            if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                m_newPasswordEdit->setAlert(true);
                m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_newPasswordEdit, 2000);
            } else {
                m_newPasswordEdit->setAlert(false);
                m_newPasswordEdit->hideAlertMessage();
            }
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
            m_securityLevelItem->setLevel(SecurityLevelItem::MidLevel);
            if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                m_newPasswordEdit->setAlert(true);
                m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_newPasswordEdit, 2000);
            } else {
                m_newPasswordEdit->setAlert(false);
                m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols"));
            }
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
            m_securityLevelItem->setLevel(SecurityLevelItem::LowLevel);
            if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                m_newPasswordEdit->setAlert(true);
                m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_newPasswordEdit, 2000);
            } else {
                m_newPasswordEdit->setAlert(false);
                m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols"), m_newPasswordEdit, 2000);
            }
        } else {
            m_newPasswordEdit->showAlertMessage(tr("Error occurred when reading the configuration files of password rules!"));
        }
    });

    connect(m_repeatPasswordEdit, &DPasswordEdit::editingFinished, this, [ = ]() {
        if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
            m_repeatPasswordEdit->setAlert(true);
            m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        }
    });

    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_oldPasswordEdit->setAccessibleName("oldpasswordedit");
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_newPasswordEdit->setAccessibleName("newpasswordedit");
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setAccessibleName("repeatpasswordedit");
    m_passwordTipsEdit->lineEdit()->setPlaceholderText(tr("Optional"));
    m_passwordTipsEdit->setAccessibleName("passwordtipsedit");

    cancleBtn->setMinimumSize(165, 36);
    saveBtn->setMinimumSize(165, 36);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);

    setFocusPolicy(Qt::StrongFocus);
}

bool ModifyPasswdPage::judgeTextEmpty(DPasswordEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), edit, 2000);
    }

    return edit->text().isEmpty();
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if ((judgeTextEmpty(m_oldPasswordEdit) && m_isCurrent) || judgeTextEmpty(m_newPasswordEdit) || judgeTextEmpty(m_repeatPasswordEdit))
        return;

    if (m_isCurrent) {
        for (auto c : m_newPasswordEdit->text()) {
            if (m_passwordTipsEdit->text().contains(c)) {
                m_passwordTipsEdit->setAlert(true);
                m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
                return;
            }
        }

        Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text(), m_repeatPasswordEdit->lineEdit()->text());
    } else {
        resetPassword(m_newPasswordEdit->text(), m_repeatPasswordEdit->text());
    }
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode, const QString &errorTxt)
{
    Q_UNUSED(exitCode)
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(),
                                                                                      m_newPasswordEdit->lineEdit()->text());
    qDebug() << "exit code:" << exitCode << "error text:" << errorTxt << "error type:" << error
            << "error tips:" << PwqualityManager::instance()->getErrorTips(error);
    if (exitCode != 0) {
        if (errorTxt.startsWith("Current Password: passwd:", Qt::CaseInsensitive)) {
            m_oldPasswordEdit->setAlert(true);
            m_oldPasswordEdit->showAlertMessage(tr("Wrong password"));
            return;
        }

        if (m_newPasswordEdit->lineEdit()->text() == m_oldPasswordEdit->lineEdit()->text()) {
            m_newPasswordEdit->setAlert(true);
            m_newPasswordEdit->showAlertMessage(tr("New password should differ from the current one"), m_oldPasswordEdit, 2000);
            return;
        }

        if (error == PW_NO_ERR) {
            if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
                m_repeatPasswordEdit->setAlert(true);
                m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
                return;
            }
        }

        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        // 企业版控制中心修改密码屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
            return;

        // 密码校验失败并且安全中心密码安全等级不为低，弹出跳转到安全中心的对话框，低、中、高等级分别对应的值为1、2、3
        QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                                     QStringLiteral("/com/deepin/defender/daemonservice"),
                                     QStringLiteral("com.deepin.defender.daemonservice"));
        QDBusReply<int> level = interface.call("GetPwdLimitLevel");
        if (!interface.isValid()) {
            return;
        }
        if (level != 1) {
            QDBusReply<QString> errorTips = interface.call("GetPwdError");
            DDialog dlg("", errorTips, this);
            dlg.setIcon(QIcon::fromTheme("preferences-system"));
            dlg.addButton(tr("Go to Settings"));
            dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
            connect(&dlg, &DDialog::buttonClicked, this, [ = ](int idx) {
                if (idx == 0) {
                    DDBusSender()
                    .service("com.deepin.defender.hmiscreen")
                    .interface("com.deepin.defender.hmiscreen")
                    .path("/com/deepin/defender/hmiscreen")
                    .method(QString("ShowPage"))
                    .arg(QString("securitytools"))
                    .arg(QString("login-safety"))
                    .call();
                }
            });
            dlg.exec();
        }
    } else if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        Q_EMIT requestChangePassword(m_curUser, m_newPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), false);
    } else {
        if (!m_passwordTipsEdit->text().simplified().isEmpty())
            requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());
        Q_EMIT requestBack();
    }
}

void ModifyPasswdPage::setPasswordEditAttribute(DPasswordEdit *edit)
{
    edit->setAttribute(Qt::WA_InputMethodEnabled, false);
    edit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[^\\x4e00-\\x9fa5]+")));
    edit->setCopyEnabled(false);
    edit->setCutEnabled(false);
}

void ModifyPasswdPage::resetPassword(const QString &password, const QString &repeatPassword)
{
    bool check = false;
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(),
                                                                                      password);

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        check = true;
    }

    if (password != repeatPassword) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        check = true;
    }

    for (auto c : password) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
            check = true;
        }
    }

    if (check)
        return;

    if (!m_passwordTipsEdit->text().simplified().isEmpty())
        requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());

    Q_EMIT requestResetPassword(m_curUser, password);
}

//在修改密码页面当前密码处设置焦点
void ModifyPasswdPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    DPasswordEdit *passwordEdit = m_isCurrent ? m_oldPasswordEdit : m_newPasswordEdit;
    if (passwordEdit && !passwordEdit->hasFocus()) {
        passwordEdit->lineEdit()->setFocus();
    }
}

void ModifyPasswdPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QWidget::paintEvent(event);
}

void ModifyPasswdPage::resetPasswordFinished(const QString &errorText)
{
    if (errorText.isEmpty()) {
        Q_EMIT requestBack();
    } else {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(errorText, m_newPasswordEdit, 2000);
    }
}

void ModifyPasswdPage::onForgetPasswordBtnClicked()
{
    m_forgetPasswordBtn->setEnabled(false);

    QString uosid;
    Q_EMIT requestUOSID(uosid);
    if (uosid.isEmpty()) {
        return;
    }

    QString uuid;
    Q_EMIT requestUUID(uuid);
    if (uuid.isEmpty()) {
        return;
    }

    Q_EMIT requestLocalBindCheck(m_curUser, uosid, uuid);
}

void ModifyPasswdPage::onStartResetPasswordReplied(const QString &errorText)
{
    if (!errorText.isEmpty()) {
        m_forgetPasswordBtn->setEnabled(true);
    } else {
        m_enableBtnTimer.singleShot(5000, this, [this]{ m_forgetPasswordBtn->setEnabled(true); });
    }
    qDebug() << "Resetpassword reply:" << errorText;
}

void ModifyPasswdPage::onLocalBindCheckUbid(const QString &ubid)
{
    if (!ubid.isEmpty()) {
        m_isBindCheckError = false;
        Q_EMIT requestStartResetPasswordExec(m_curUser);
    }
    if (ubid.isEmpty() && !m_isBindCheckError) {
        UnionIDBindReminderDialog dlg;
        dlg.exec();
    }
}

void ModifyPasswdPage::onLocalBindCheckError(const QString &error)
{
    m_isBindCheckError = true;
    QString tips;
    if (error.contains("7500")) {
        tips = tr("System error");
    } else if (error.contains("7506")) {
        tips = tr("Network error");
    }
    if (!tips.isEmpty()) {
        DMessageManager::instance()->sendMessage(this,
                                                 style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 tips);
    }
}
