/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "modifypasswordpage.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QSettings>

using namespace dcc::widgets;
using namespace dcc::accounts;

ModifyPasswordPage::ModifyPasswordPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userInter(user),

      m_oldpwdEdit(new LineEditWidget),
      m_pwdEdit(new LineEditWidget),
      m_pwdEditRepeat(new LineEditWidget),

      m_buttonTuple(new ButtonTuple)
{
    m_oldpwdEdit->textEdit()->setEchoMode(QLineEdit::Password);
    m_oldpwdEdit->setTitle(tr("Current Password"));
    m_pwdEdit->textEdit()->setEchoMode(QLineEdit::Password);
    m_pwdEdit->setTitle(tr("New Password"));
    m_pwdEditRepeat->textEdit()->setEchoMode(QLineEdit::Password);
    m_pwdEditRepeat->setTitle(tr("Repeat Password"));

    m_oldpwdEdit->setPlaceholderText(tr("Required"));
    m_pwdEdit->setPlaceholderText(tr("Required"));
    m_pwdEditRepeat->setPlaceholderText(tr("Required"));

    QPushButton *cancel = m_buttonTuple->leftButton();
    QPushButton *accept = m_buttonTuple->rightButton();
    cancel->setText(tr("Cancel"));
    cancel->setAccessibleName("Modify_Cancel");
    accept->setText(tr("Accept"));
    accept->setAccessibleName("Modify_Accept");

    SettingsGroup *pwdGroup = new SettingsGroup;
    pwdGroup->appendItem(m_oldpwdEdit);
    pwdGroup->appendItem(m_pwdEdit);
    pwdGroup->appendItem(m_pwdEditRepeat);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(pwdGroup);
    mainLayout->addWidget(m_buttonTuple);
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);

    TranslucentFrame *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);

    connect(accept, &QPushButton::clicked, this, &ModifyPasswordPage::passwordSubmit);
    connect(cancel, &QPushButton::clicked, this, &ModifyPasswordPage::back);
    connect(m_pwdEditRepeat->textEdit(), &QLineEdit::editingFinished, this, &ModifyPasswordPage::checkPwd);
    connect(user, &User::passwordModifyFinished, this, &ModifyPasswordPage::onPasswordChangeFinished);
    connect(user, &User::fullnameChanged, this, &ModifyPasswordPage::updateTitle);
    connect(user, &User::passwordStatusChanged, this, [=] (const QString& status) {
            m_oldpwdEdit->setVisible(status != NO_PASSWORD);
    });
    connect(this, &ModifyPasswordPage::disappear, this, &ModifyPasswordPage::hideAlert);
    connect(m_oldpwdEdit->textEdit(), &QLineEdit::editingFinished, this, &ModifyPasswordPage::hideAlert);
    connect(m_pwdEdit->textEdit(), &QLineEdit::editingFinished, this, [=] {
        onEditFinished<LineEditWidget*>(m_pwdEdit);
    });
    connect(m_pwdEditRepeat->textEdit(), &QLineEdit::editingFinished, this, [=] {
        if (!m_pwdEdit->isShowAlert()) {
            onEditFinished<LineEditWidget*>(m_pwdEditRepeat);
        }
    });

    m_oldpwdEdit->setVisible(user->passwordStatus() != NO_PASSWORD);

    updateTitle();
}

void ModifyPasswordPage::passwordSubmit()
{
    const QString pwdOld = m_oldpwdEdit->textEdit()->text();
    const QString pwd0 = m_pwdEdit->textEdit()->text();
    const QString pwd1 = m_pwdEditRepeat->textEdit()->text();

    checkPwd();

    if (m_oldpwdEdit->text() == m_pwdEdit->text()) {
        m_pwdEdit->showAlertMessage(tr("New password should differ from the current one"));
        return;
    }

    if ((m_userInter->passwordStatus() != NO_PASSWORD && m_oldpwdEdit->text().isEmpty()) || m_pwdEditRepeat->text().isEmpty() || m_pwdEdit->text().isEmpty())
        return;

    if (pwd0 != pwd1)
        return;

    Q_EMIT requestChangePassword(m_userInter, pwdOld, pwd0);
}

void ModifyPasswordPage::checkPwd()
{
    if (m_userInter->passwordStatus() == "P") {
        m_oldpwdEdit->setIsErr(m_oldpwdEdit->text().isEmpty());
    }

    m_pwdEdit->setIsErr(m_pwdEdit->text().isEmpty() || m_oldpwdEdit->text() == m_pwdEdit->text());
    m_pwdEditRepeat->setIsErr(m_pwdEditRepeat->text() != m_pwdEdit->text());
}

void ModifyPasswordPage::onPasswordChangeFinished(const int exitCode)
{
    if (exitCode == 0)
    {
        Q_EMIT back();
        return;
    } else {
        qWarning() << Q_FUNC_INFO << "exit =" << exitCode;

        m_oldpwdEdit->setIsErr();
    }
}

void ModifyPasswordPage::updateTitle()
{
    const QString &fullname = m_userInter->fullname();
    setTitle(tr("Password") + " - " + (fullname.isEmpty() ? m_userInter->name() : fullname));
}

void ModifyPasswordPage::hideAlert()
{
    m_oldpwdEdit->hideAlertMessage();
    m_pwdEdit->hideAlertMessage();
    m_pwdEditRepeat->hideAlertMessage();
}

bool ModifyPasswordPage::validatePassword(const QString &password)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    bool strong_password_check = setting.value("STRONG_PASSWORD", false).toBool();

    if (!strong_password_check) return true;

    if (password.size() < 7 || password.size() > 16) return false;

    uint success_num = 0;

    const QStringList strong_policy_list {
        "1234567890",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "~!@#$%^&*()[]{}\\|/?,.<>"
    };

    for (const QString &policy : strong_policy_list) {
        if (ContainsChar(password, policy)) {
            ++success_num;
        }
    }

    return success_num > 1;
}

bool ModifyPasswordPage::ContainsChar(const QString &password, const QString &validate)
{
    for (const QString &p : password) {
        if (validate.contains(p)) {
            return true;
        }
    }

    return false;
}

template <typename T>
void ModifyPasswordPage::onEditFinished(T t)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    if (!setting.value("STRONG_PASSWORD", false).toBool()) return t->hideAlertMessage();

    const QString &password = t->text();

    if (m_userInter->name().toLower() == password.toLower()) {
        t->showAlertMessage(tr("The password should be different from the username"));
        return;
    }

    if (!validatePassword(password)) {
        m_oldpwdEdit->hideAlertMessage();
        m_pwdEdit->hideAlertMessage();
        m_pwdEditRepeat->hideAlertMessage();
        t->showAlertMessage(tr("The password must contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
    }
    else {
        t->hideAlertMessage();
    }
}
