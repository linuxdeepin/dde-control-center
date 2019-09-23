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
    , m_mainContentLayout(new QVBoxLayout)
    , m_cansaveLayout(new QHBoxLayout)
    , m_titleLabel(new QLabel)
    , m_oldPasswdLabel(new QLabel)
    , m_newPasswdLabel(new QLabel)
    , m_repeatPasswdLabel(new QLabel)
    , m_cancleBtn(new QPushButton)
    , m_saveBtn(new DSuggestButton)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
{
    initWidget();
    initData();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
}

void ModifyPasswdPage::initWidget()
{
    m_cansaveLayout->addWidget(m_cancleBtn, 0, Qt::AlignCenter);
    m_cansaveLayout->addWidget(m_saveBtn, 0, Qt::AlignCenter);

    m_mainContentLayout->addSpacing(40);
    m_mainContentLayout->addWidget(m_titleLabel, 0, Qt::AlignHCenter);
    m_mainContentLayout->addSpacing(40);

    m_mainContentLayout->addWidget(m_oldPasswdLabel);
    m_mainContentLayout->addWidget(m_oldPasswordEdit);

    m_mainContentLayout->addWidget(m_newPasswdLabel);
    m_mainContentLayout->addWidget(m_newPasswordEdit);

    m_mainContentLayout->addWidget(m_repeatPasswdLabel);
    m_mainContentLayout->addWidget(m_repeatPasswordEdit);

    m_mainContentLayout->addStretch();

    m_mainContentLayout->addLayout(m_cansaveLayout);

    setLayout(m_mainContentLayout);
    setFocusPolicy(Qt::StrongFocus);
}

void ModifyPasswdPage::initData()
{
    m_titleLabel->setText(tr("Change Password"));

    m_oldPasswdLabel->setText(tr("Current Password"));
    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));

    m_newPasswdLabel->setText(tr("New Password"));
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));

    m_repeatPasswdLabel->setText(tr("Repeat Password"));
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));

    m_cancleBtn->setText(tr("Cancel"));
    m_saveBtn->setText(tr("Save"));

    connect(m_cancleBtn, &QPushButton::clicked, this, &ModifyPasswdPage::requestBack);

    connect(m_saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });

    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert()) {
            m_oldPasswordEdit->hideAlertMessage();
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_newPasswordEdit->isAlert()) {
            m_newPasswordEdit->hideAlertMessage();
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert()) {
            m_repeatPasswordEdit->hideAlertMessage();
        }
    });
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if (!onPasswordEditFinished(m_oldPasswordEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_newPasswordEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_repeatPasswordEdit)) {
        return;
    }

    Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text());
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode)
{
    if (exitCode == ModifyPasswdPage::ModifyNewPwdSuccess) {
        Q_EMIT requestBack(true);
        return;
    } if (exitCode == ModifyPasswdPage::InputOldPwdError) {
        m_oldPasswordEdit->showAlertMessage(tr("Wrong password"));
        return;
    } else {
        qWarning() << Q_FUNC_INFO << "exit =" << exitCode;
    }
}

bool ModifyPasswdPage::validatePassword(const QString &password)
{
    QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                              QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<>");

    return containsChar(password, validate_policy);
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

bool ModifyPasswdPage::onPasswordEditFinished(Dtk::Widget::DPasswordEdit *edit)
{
    const QString &password = edit->lineEdit()->text();

    if (password.isEmpty()) {
        if (edit == m_oldPasswordEdit && m_curUser->passwordStatus() != NO_PASSWORD) {
            edit->showAlertMessage(tr("Wrong password"));
        } else {
            edit->showAlertMessage(tr("Password cannot be empty"));
        }
        return false;
    }

    if (m_curUser->name().toLower() == password.toLower()) {
        edit->showAlertMessage(tr("The password should be different from the username"));
        return false;
    }

    if (!validatePassword(password)) {
        edit->showAlertMessage(tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"));
        return false;
    }

    //新密码
    if (edit == m_newPasswordEdit) {
        if (m_oldPasswordEdit->lineEdit()->text() == password) {
            edit->showAlertMessage(tr("New password should differ from the current one"));
            return false;
        }
    }

    //重复密码
    if (edit == m_repeatPasswordEdit) {
        if (m_newPasswordEdit->lineEdit()->text() != password) {
            edit->showAlertMessage(tr("Passwords do not match"));
            return false;
        }
    }

    return true;
}
