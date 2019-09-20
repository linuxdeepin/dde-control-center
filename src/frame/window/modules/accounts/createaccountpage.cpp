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

#include "createaccountpage.h"

#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QTimer>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout)
    , m_titleLayout(new QHBoxLayout)
    , m_inputLayout(new QVBoxLayout)
    , m_selectLayout(new QHBoxLayout)
    , m_title(new QLabel)
    , m_avatarListWidget(new AvatarListWidget)
    , m_nameLabel(new QLabel)
    , m_fullnameLabel(new QLabel)
    , m_passwdLabel(new QLabel)
    , m_repeatpasswdLabel(new QLabel)
    , m_nameEdit(new QLineEdit)
    , m_fullnameEdit(new QLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
    , m_cancleBtn(new QPushButton)
    , m_addBtn(new QPushButton)
    , m_errorTip(new ErrorTip)
{
    initWidgets();
    initDatas();
}

CreateAccountPage::~CreateAccountPage()
{
    m_errorTip->deleteLater();
}

void CreateAccountPage::initWidgets()
{
    m_titleLayout->addWidget(m_title, 0, Qt::AlignCenter);

    m_inputLayout->setSpacing(3);

    m_inputLayout->addWidget(m_nameLabel);
    m_inputLayout->addWidget(m_nameEdit);

    m_inputLayout->addWidget(m_fullnameLabel);
    m_inputLayout->addWidget(m_fullnameEdit);

    m_inputLayout->addWidget(m_passwdLabel);
    m_inputLayout->addWidget(m_passwdEdit);

    m_inputLayout->addWidget(m_repeatpasswdLabel);
    m_inputLayout->addWidget(m_repeatpasswdEdit);

    m_selectLayout->addWidget(m_cancleBtn, 0, Qt::AlignCenter);
    m_selectLayout->addWidget(m_addBtn, 0, Qt::AlignCenter);

    m_mainContentLayout->addLayout(m_titleLayout);
    m_mainContentLayout->addWidget(m_avatarListWidget);
    m_mainContentLayout->addLayout(m_inputLayout);
    m_mainContentLayout->addLayout(m_selectLayout);
    m_mainContentLayout->addStretch();

    setLayout(m_mainContentLayout);
    setFocusPolicy(Qt::StrongFocus);

    m_errorTip->setWindowFlags(Qt::ToolTip);
    m_errorTip->hide();
}

void CreateAccountPage::initDatas()
{
    connect(m_cancleBtn, &QPushButton::clicked, this, &CreateAccountPage::requestBack);
    connect(m_addBtn, &QPushButton::clicked, this, &CreateAccountPage::createUser);
    connect(m_nameEdit, &QLineEdit::editingFinished, this, &CreateAccountPage::onNameEditFinished);
    connect(m_nameEdit, &QLineEdit::textEdited, this, [ = ](const QString & str) {
        m_nameEdit->setText(str.toLower());
    });

    connect(m_passwdEdit, &DPasswordEdit::editingFinished, this, [ = ] {
        onEditFinished(m_passwdEdit);
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::editingFinished, this, [ = ] {
        if (!m_errorTip->isVisible())
        {
            onEditFinished(m_repeatpasswdEdit);
        }
    });

    //~ contents_path /accounts/New Account
    m_title->setText(tr("New Account"));
    m_nameLabel->setText(tr("Username"));
    m_nameEdit->setPlaceholderText(tr("Required"));
    m_fullnameLabel->setText(tr("Full Name"));
    m_fullnameEdit->setPlaceholderText(tr("Selected"));//选填
    m_passwdLabel->setText(tr("Password"));
    m_passwdEdit->setPlaceholderText(tr("Required"));
    m_repeatpasswdLabel->setText(tr("Repeat Password"));
    m_repeatpasswdEdit->setPlaceholderText(tr("Required"));

    m_cancleBtn->setText(tr("Cancel"));
    m_addBtn->setText(tr("Create"));
}

void CreateAccountPage::setModel(User *user)
{
    m_newUser = user;
    Q_ASSERT(m_newUser);
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, m_newUser, &User::setCurrentAvatar);
}

void CreateAccountPage::createUser()
{
    if (m_nameEdit->text().isEmpty()) {
        showErrorTip(m_nameEdit, tr("Username cannot be empty"));
        return;
    }
    if (m_passwdEdit->text().isEmpty()) {
        showErrorTip(m_passwdEdit, tr("Password cannot be empty"));
        return;
    }
    if (m_passwdEdit->text() != m_repeatpasswdEdit->text()) {
        showErrorTip(m_repeatpasswdEdit, tr("Passwords do not match"));
        return;
    }

    //随机分配图像 [0, 13]
    int random = qrand() % 14;
    m_newUser->setCurrentAvatar(m_avatarListWidget->getAvatarPath(random));
    m_newUser->setName(m_nameEdit->text());
    m_newUser->setFullname(m_fullnameEdit->text());
    m_newUser->setPassword(m_passwdEdit->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->text());

    Q_EMIT requestCreateUser(m_newUser);
//    Q_EMIT requestBack();
}

bool CreateAccountPage::validatePassword(const QString &password)
{
    QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                              QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<>");

    return containsChar(password, validate_policy);
}

bool CreateAccountPage::containsChar(const QString &password, const QString &validate)
{
    for (const QChar &p : password) {
        if (!validate.contains(p)) {
            return false;
        }
    }

    return true;
}

void CreateAccountPage::showErrorTip(QLineEdit *edit, const QString &error)
{
    QPoint globalStart = edit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + edit->width() / 2,
                     globalStart.y() + edit->height() / 2 + 10);
    QTimer::singleShot(1.5 * 1000, this, [&]() {
        m_errorTip->hide();
    });
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        break;
    case CreationResult::UserNameError:
        showErrorTip(m_nameEdit, result->message());
        break;
    case CreationResult::PasswordError:
        showErrorTip(m_passwdEdit, result->message());
        break;
    case CreationResult::PasswordMatchError:
        showErrorTip(m_repeatpasswdEdit, result->message());
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        break;
    }

    result->deleteLater();
}

void CreateAccountPage::onEditFinished(DPasswordEdit *edit)
{
    const QString &password = edit->text();
    if (password.isEmpty()) {
        showErrorTip(edit, tr("Password cannot be empty"));
        return;
    }

    if (m_nameEdit->text().toLower() == password.toLower()) {
        showErrorTip(edit, tr("The password should be different from the username"));
        return;
    }

    bool result = validatePassword(password);
    if (!result) {
        showErrorTip(edit, tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"));
        m_addBtn->setEnabled(false);
    } else {
        if (m_errorTip->isVisible()) {
            m_errorTip->hide();
        }
        m_addBtn->setEnabled(true);
    }
}

bool CreateAccountPage::validateUsername(const QString &username)
{
    const QString name_validate = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("-_");
    return containsChar(username, name_validate);
}

void CreateAccountPage::onNameEditFinished()
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
    QString username = edit->text();

    if (edit->text().size() < 3 || edit->text().size() > 32) {
        showErrorTip(edit, tr("Username must be between 3 and 32 characters"));
        return;
    }

    const QString compStr = "abcdefghijklmnopqrstuvwxyz";
    if (!compStr.contains(username.at(0))) {
        showErrorTip(edit, tr("The first character must be in lower case"));
        return;
    }

    if (!validateUsername(username)) {
        showErrorTip(edit, tr("Username must only contain a~z, 0~9, - or _"));
        return;
    }

    if (m_errorTip->isVisible()) {
        m_errorTip->hide();
    }
}
