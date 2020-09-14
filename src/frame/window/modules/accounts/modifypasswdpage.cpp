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

#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QDebug>

using namespace dcc::accounts;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

ModifyPasswdPage::ModifyPasswdPage(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
{
    initWidget();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
}

void ModifyPasswdPage::initWidget()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addSpacing(40);

    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    mainContentLayout->addSpacing(40);

    QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
    mainContentLayout->addWidget(oldPasswdLabel);
    mainContentLayout->addWidget(m_oldPasswordEdit);

    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    mainContentLayout->addWidget(newPasswdLabel);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->setSpacing(10);
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    mainContentLayout->addLayout(cansaveLayout);
    setLayout(mainContentLayout);
    cancleBtn->setDefault(true);
    saveBtn->setDefault(true);
    cancleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });

    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert()) {
            m_oldPasswordEdit->hideAlertMessage();
            m_oldPasswordEdit->setAlert(false);
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_newPasswordEdit->isAlert()) {
            m_newPasswordEdit->hideAlertMessage();
            m_newPasswordEdit->setAlert(false);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert()) {
            m_repeatPasswordEdit->hideAlertMessage();
            m_repeatPasswordEdit->setAlert(false);
        }
    });

    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_oldPasswordEdit->setAccessibleName("oldpasswordedit");
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_newPasswordEdit->setAccessibleName("newpasswordedit");
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setAccessibleName("repeatpasswordedit");

    cancleBtn->setMinimumSize(165, 36);
    saveBtn->setMinimumSize(165, 36);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);

    setFocusPolicy(Qt::StrongFocus);
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if (m_oldPasswordEdit->lineEdit()->text().isEmpty()) {
        m_oldPasswordEdit->setAlert(true);
        m_oldPasswordEdit->showAlertMessage(tr("Password cannot be empty"));
        return;
    }
    if (!onPasswordEditFinished(m_newPasswordEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_repeatPasswordEdit)) {
        return;
    }

    DaemonService *daemonservice = new DaemonService("com.deepin.defender.daemonservice",
                                                     "/com/deepin/defender/daemonservice",
                                                     QDBusConnection::sessionBus(), this);
    QString strPwd = m_newPasswordEdit->lineEdit()->text();
    if (strPwd.length() >= daemonservice->GetPwdLen() && m_curUser->charactertypes(strPwd) >= daemonservice->GetPwdTypeLen()) {
        Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text());
    } else {
        DDialog dlg("", daemonservice->GetPwdError());
        dlg.setIcon(QIcon::fromTheme("preferences-system"));
        dlg.addButton(tr("Go to Settings"));
        dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
        connect(&dlg, &DDialog::buttonClicked, this, [this](int idx){
            if (idx == 0) {
                Defender *defender = new Defender("com.deepin.defender.hmiscreen",
                                                  "/com/deepin/defender/hmiscreen",
                                                  QDBusConnection::sessionBus(), this);
                defender->ShowModule("systemsafety");
            }
        });
        dlg.exec();
    }
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode)
{
    if (exitCode == ModifyPasswdPage::ModifyNewPwdSuccess) {
        DaemonService *daemonservice = new DaemonService("com.deepin.defender.daemonservice",
                                                         "/com/deepin/defender/daemonservice",
                                                         QDBusConnection::sessionBus(), this);
        daemonservice->PasswordUpdate();

        Q_EMIT requestBack(AccountsWidget::ModifyPwdSuccess);
        return;
    } if (exitCode == ModifyPasswdPage::InputOldPwdError) {
        m_oldPasswordEdit->setAlert(true);
        m_oldPasswordEdit->showAlertMessage(tr("Wrong password"), -1);
        return;
    } else {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(tr("Failed to change the password"), -1);
        qDebug() << Q_FUNC_INFO << "exit =" << exitCode;
    }
}

//在修改密码页面当前密码处设置焦点
void ModifyPasswdPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (m_oldPasswordEdit && !m_oldPasswordEdit->hasFocus()) {
        m_oldPasswordEdit->lineEdit()->setFocus();
    }
}

bool ModifyPasswdPage::onPasswordEditFinished(Dtk::Widget::DPasswordEdit *edit)
{
    CreateAccountPage::PassWordType passwordtype = CreateAccountPage::NormalPassWord;
    const QString &password = edit->lineEdit()->text();
    QGSettings setting("com.deepin.dde.control-center", QByteArray());
    if (setting.get("account-password-type").toString() == "IncludeBlankSymbol")
        passwordtype = CreateAccountPage::IncludeBlankSymbol;

    if (password.isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"));
        return false;
    }

    //重复密码
    if (edit == m_repeatPasswordEdit) {
        if (m_newPasswordEdit->lineEdit()->text() != password) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Passwords do not match"), -1);
            return false;
        }
    }

    int passResult = PwqualityManager::instance()->verifyPassword(password);
    QString blanksymbolstr =  tr("The password must have at least %1 characters, and contain at least %2 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols")
            .arg(PwqualityManager::instance()->getPasswordMinLength())
            .arg(PwqualityManager::instance()->getValidateRequiredString());

    switch (passResult)
    {
    case ENUM_PASSWORD_NOTEMPTY:
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), -1);
        return false;
    case ENUM_PASSWORD_TOOSHORT:
        edit->setAlert(true);
        if (passwordtype == CreateAccountPage::NormalPassWord)
            edit->showAlertMessage(tr("The password must have at least %1 characters").arg(PwqualityManager::instance()->getPasswordMinLength()), -1);
        if (passwordtype == CreateAccountPage::IncludeBlankSymbol)
            edit->showAlertMessage(blanksymbolstr, -1);
        return false;
    case ENUM_PASSWORD_TOOLONG:
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password must be no more than %1 characters").arg(PwqualityManager::instance()->getPasswordMaxLength()), -1);
        return false;
    case ENUM_PASSWORD_TYPE:
        edit->setAlert(true);
        if (passwordtype == CreateAccountPage::NormalPassWord)
            edit->showAlertMessage(tr("The password should contain at least %1 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(PwqualityManager::instance()->getValidateRequiredString()), -1);
        if (passwordtype == CreateAccountPage::IncludeBlankSymbol)
            edit->showAlertMessage(blanksymbolstr, -1);
        return false;
    case ENUM_PASSWORD_CHARACTER:
        edit->setAlert(true);
        if (passwordtype == CreateAccountPage::NormalPassWord)
            edit->showAlertMessage(tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"), -1);
        if (passwordtype == CreateAccountPage::IncludeBlankSymbol)
            edit->showAlertMessage(blanksymbolstr, -1);
        return false;
    case ENUM_PASSWORD_SEVERAL:
        edit->setAlert(true);
        edit->showAlertMessage(blanksymbolstr, -1);
        return false;
    case ENUM_PASSWORD_PALINDROME:
        if (passwordtype == CreateAccountPage::NormalPassWord) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Password must not contain more than 4 palindrome characters"));
            return false;
        }
        break;
    case ENUM_PASSWORD_DICT_FORBIDDEN:
        if (passwordtype == CreateAccountPage::NormalPassWord) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Password must not contain common words and combinations"));
            return false;
        }
        break;
    }

    if (m_oldPasswordEdit->lineEdit()->text() == password) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("New password should differ from the current one"), -1);
        return false;
    }

    const int maxSize = 512;
    if (password.size() > maxSize) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password must be no more than %1 characters").arg(maxSize), -1);
        return false;
    }

    if (passwordtype == CreateAccountPage::IncludeBlankSymbol) {
        QString reversusername;
        QStringList reversenamelist;

        for (int i = m_curUser->name().count() - 1; i > -1; i--) {
            reversenamelist << m_curUser->name().at(i);
        }
        reversusername = reversenamelist.join("");

        //密码不可为用户名重复或倒置
        if (password == m_curUser->name() || password == reversusername) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Password should not be the repeated or reversed username"));
            return false;
        }
    }

    return true;
}
