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
#include "pwquality_manager.h"
#include "../../utils.h"

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
    , m_passwordMinLength(-1)
    , m_passwordMaxLength(-1)
    , m_validateRequiredString(-1)

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
        qDebug() << Q_FUNC_INFO << "exit =" << exitCode;
    }
}

int  ModifyPasswdPage::passwordCompositionType(const QStringList &validate, const QString &password)
{
    return static_cast<int>(std::count_if(validate.cbegin(), validate.cend(),
                                          [=](const QString &policy) {
                                              for (const QChar &c : policy) {
                                                  if (password.contains(c)) {
                                                      return true;
                                                  }
                                              }
                                              return false;
                                          }));
}

int ModifyPasswdPage::verifyPassword(const QString &password)
{
    QFileInfo fileInfo("/etc/deepin/dde.conf");
    if (fileInfo.isFile()) {
        // NOTE(justforlxz): 配置文件由安装器生成，后续改成PAM模块
        QSettings setting("/etc/deepin/dde.conf", QSettings::IniFormat);
        setting.beginGroup("Password");
        const bool strong_password_check = setting.value("STRONG_PASSWORD", false).toBool();
        m_passwordMinLength   = setting.value("PASSWORD_MIN_LENGTH").toInt();
        m_passwordMaxLength   = setting.value("PASSWORD_MAX_LENGTH").toInt();
        const QStringList validate_policy= setting.value("VALIDATE_POLICY").toString().split(";");
        m_validateRequiredString      = setting.value("VALIDATE_REQUIRED").toInt();
        QString validate_policy_string = setting.value("VALIDATE_POLICY").toString();

        if (!strong_password_check) {
            return _ENUM_PASSWORD_CHARACTER;
        }
        if (password.size() == 0) {
            return _ENUM_PASSWORD_NOTEMPTY;
        }
        if (password.size() > 0 && password.size() < m_passwordMinLength) {
            return _ENUM_PASSWORD_TOOSHORT;
        } 
        if (passwordCompositionType(validate_policy, password) < m_validateRequiredString) {
            if (password.size() < m_passwordMinLength) {
                return _ENUM_PASSWORD_SEVERAL;
            }
            if (!(password.split("").toSet() - validate_policy.join("").split("").toSet()).isEmpty()) {
                return _ENUM_PASSWORD_CHARACTER;
            }
            return _ENUM_PASSWORD_TYPE;
        } 
        
        if (password.size() > m_passwordMaxLength) {
            return _ENUM_PASSWORD_TOOLONG;
        }
        if (!containsChar(password, validate_policy_string)) {
            return _ENUM_PASSWORD_CHARACTER;
        }
        if (IsServerSystem) {
            if (!PwqualityManager::instance()->palindromeChecked(password).isEmpty()) {
                return _ENUM_PASSWORD_PALINDROME;
            }

            int secChkLeastLen=6;
            int length = password.length();
            for (int i = 0; i < length - secChkLeastLen + 1; i++) {
                for (int j = secChkLeastLen; i + j <= length; j++) {
                    QString subStr = password.mid(i, j);
                    QString sChkResult = PwqualityManager::instance()->dictChecked(subStr);
                    if (!sChkResult.isEmpty()) {
                        return _ENUM_PASSWORD_DICT_FORBIDDEN;
                    }
                }
            }
            return _ENUM_PASSWORD_SUCCESS;
        } 
        return _ENUM_PASSWORD_SUCCESS;
    } else {
        QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                                      QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<>");
        bool ret = containsChar(password, validate_policy);
        if (!ret) {
            return _ENUM_PASSWORD_CHARACTER;
        }
        return _ENUM_PASSWORD_SUCCESS;
    }
}

bool ModifyPasswdPage::containsChar(const QString &password, const QString &validate)
{
    for (const QChar &p : password) {
        if (!validate.contains(p)) {
            return false;
        }
    }

    return true;
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
    const QString &password = edit->lineEdit()->text();

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

    int passResult = verifyPassword(password);
    switch (passResult)
    {
    case _ENUM_PASSWORD_NOTEMPTY:
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), -1);
        return false;
    case _ENUM_PASSWORD_TOOSHORT:
        edit->setAlert(true);
        edit->showAlertMessage(tr("The password must have at least %1 characters").arg(m_passwordMinLength), -1);
        return false;
    case _ENUM_PASSWORD_TOOLONG:
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password must be no more than %1 characters").arg(m_passwordMaxLength), -1);
        return false;
    case _ENUM_PASSWORD_TYPE:
        edit->setAlert(true);
        edit->showAlertMessage(tr("The password should contain at least %1 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_validateRequiredString), -1);
        return false;
    case _ENUM_PASSWORD_CHARACTER:
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"), -1);
        return false;
    case _ENUM_PASSWORD_SEVERAL:
        edit->setAlert(true);
        edit->showAlertMessage(tr("The password must have at least %1 characters, and contain at least %2 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_passwordMinLength).arg(m_validateRequiredString), -1);
        return false;
    case _ENUM_PASSWORD_PALINDROME:
        edit->setAlert(true);
        edit->showAlertMessage(tr("The password cannot contain palindrome"));
        return false;
    case _ENUM_PASSWORD_DICT_FORBIDDEN:
        edit->setAlert(true);
        edit->showAlertMessage(tr("The password cannot use the common words"));
        return false;
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

    return true;
}
