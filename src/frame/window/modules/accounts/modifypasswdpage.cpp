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
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QList>

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
    , m_saveBtn(new QPushButton)
    , m_errorTip(new ErrorTip)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
{
    initWidget();
    initData();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
    m_errorTip->deleteLater();
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

    m_passeditList.push_back(m_oldPasswordEdit);
    m_passeditList.push_back(m_newPasswordEdit);
    m_passeditList.push_back(m_repeatPasswordEdit);

    m_errorTip->setWindowFlags(Qt::ToolTip);
    m_errorTip->hide();
}

void ModifyPasswdPage::initData()
{
    m_titleLabel->setText(tr("Change Password"));

    m_oldPasswdLabel->setText(tr("Current Password"));
    m_oldPasswordEdit->setPlaceholderText(tr("Required"));

    m_newPasswdLabel->setText(tr("New Password"));
    m_newPasswordEdit->setPlaceholderText(tr("Required"));

    m_repeatPasswdLabel->setText(tr("Repeat Password"));
    m_repeatPasswordEdit->setPlaceholderText(tr("Required"));

    m_cancleBtn->setText(tr("Cancel"));
    m_saveBtn->setText(tr("Save"));

    connect(m_cancleBtn, &QPushButton::clicked, this, &ModifyPasswdPage::requestBack);

    connect(m_saveBtn, &QPushButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });

    for (auto edit : m_passeditList) {
        connect(edit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
        connect(edit, &QLineEdit::editingFinished, this, &ModifyPasswdPage::onDoEditFinish, Qt::QueuedConnection);
        Q_EMIT edit->editingFinished();
    }
}

void ModifyPasswdPage::clickSaveBtn()
{
    QString oldpwd = m_oldPasswordEdit->text();
    QString newpwd = m_newPasswordEdit->text();
    QString repeatpwd = m_repeatPasswordEdit->text();

    if ((m_curUser->passwordStatus() != NO_PASSWORD && m_oldPasswordEdit->text().isEmpty())) {
        showErrorTip(m_oldPasswordEdit, tr("Wrong password"));
        return;
    }

    if (m_newPasswordEdit->text().isEmpty()) {
        showErrorTip(m_newPasswordEdit, tr("Password cannot be empty"));
        return;
    }

    if (m_repeatPasswordEdit->text().isEmpty()) {
        showErrorTip(m_repeatPasswordEdit, tr("Password cannot be empty"));
        return;
    }

    if (oldpwd == newpwd) {
        showErrorTip(m_newPasswordEdit, tr("New password should differ from the current one"));
        return;
    }

    if (newpwd != repeatpwd) {
        showErrorTip(m_repeatPasswordEdit, tr("Passwords do not match"));
        return;
    }

    Q_EMIT requestChangePassword(m_curUser, oldpwd, newpwd);
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode)
{
    if (exitCode == ModifyPasswdPage::ModifyNewPwdSuccess) {
        Q_EMIT requestBack();
        return;
    } if (exitCode == ModifyPasswdPage::InputOldPwdError) {
        showErrorTip(m_oldPasswordEdit, tr("Wrong password"));
        return;
    } else {
        qWarning() << Q_FUNC_INFO << "exit =" << exitCode;
    }
}

bool ModifyPasswdPage::validatePassword(const QString &password)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    bool strong_password_check = setting.value("STRONG_PASSWORD", false).toBool();

    if (!strong_password_check) {
        return true;
    }

    if (password.size() < 7 || password.size() > 16) {
        return false;
    }

    uint success_num = 0;

    const QStringList strong_policy_list {
        "1234567890",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "~!@#$%^&*()[]{}\\|/?,.<>"
    };

    for (const QString &policy : strong_policy_list) {
        if (containsChar(password, policy)) {
            ++success_num;
        }
    }

    return success_num > 1;
}

bool ModifyPasswdPage::containsChar(const QString &password, const QString &validate)
{
    for (auto p : password) {
        if (validate.contains(p)) {
            return true;
        }
    }

    return false;
}

void ModifyPasswdPage::showErrorTip(QLineEdit *edit, const QString &error)
{
    QPoint globalStart = edit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + edit->width() / 2,
                     globalStart.y() + edit->height() / 2 + 10);
    QTimer::singleShot(1.5 * 1000, this, [&]() {
        m_errorTip->hide();
    });
}

void ModifyPasswdPage::onDoEditFinish()
{
    DPasswordEdit *edit = qobject_cast<DPasswordEdit *>(sender());
    onEditFinished(edit);
}

void ModifyPasswdPage::onEditFinished(Dtk::Widget::DPasswordEdit *t)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");

    if (!setting.value("STRONG_PASSWORD", false).toBool()) {
        return m_errorTip->hide();
    }

    const QString &password = t->text();

    if (m_curUser->name().toLower() == password.toLower()) {
        showErrorTip(t, tr("The password should be different from the username"));
        return;
    }

    if (!validatePassword(password)) {
        showErrorTip(t, tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"));
    } else {
        m_errorTip->hide();
    }
}
