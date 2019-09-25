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
    , m_errorEdit(nullptr)
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

    connect(m_nameEdit, &QLineEdit::textEdited, this, [ = ](const QString & str) {
        if (m_nameEdit == m_errorEdit && m_errorTip->isVisible()) {
            m_errorTip->hide();
        }
        m_nameEdit->setText(str.toLower());
    });

    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit == m_errorEdit && m_errorTip->isVisible()) {
            m_errorTip->hide();
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit == m_errorEdit && m_errorTip->isVisible()) {
            m_errorTip->hide();
        }
    });

    //~ contents_path /accounts/New Account
    m_title->setText(tr("New Account"));
    m_nameLabel->setText(tr("Username"));
    m_nameEdit->setPlaceholderText(tr("Required"));//必填
    m_fullnameLabel->setText(tr("Full Name"));
    m_fullnameEdit->setPlaceholderText(tr("optional"));//选填
    m_passwdLabel->setText(tr("Password"));
    m_passwdEdit->setPlaceholderText(tr("Required"));//必填
    m_repeatpasswdLabel->setText(tr("Repeat Password"));
    m_repeatpasswdEdit->setPlaceholderText(tr("Required"));//必填

    m_cancleBtn->setText(tr("Cancel"));
    m_addBtn->setText(tr("Create"));
}

void CreateAccountPage::setModel(User *user)
{
    m_newUser = user;
    Q_ASSERT(m_newUser);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!onNameEditFinished()) { return; }
    if (!onPasswordEditFinished(m_passwdEdit)) { return; }
    if (!onPasswordEditFinished(m_repeatpasswdEdit)) { return; }

    //如果用户没有选图像
    int index = m_avatarListWidget->getCurrentSelectIndex();
    if (index == -1) {
        //随机分配图像 [0, 13]
        index = qrand() % 14;
    }
    m_newUser->setCurrentAvatar(m_avatarListWidget->getAvatarPath(index));
    m_newUser->setName(m_nameEdit->text());
    m_newUser->setFullname(m_fullnameEdit->text());
    m_newUser->setPassword(m_passwdEdit->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->text());

    Q_EMIT requestCreateUser(m_newUser);
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
    m_errorEdit = edit;//记录当前是哪个输入框提示出错
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

bool CreateAccountPage::onPasswordEditFinished(DPasswordEdit *edit)
{
    const QString &userpassword = edit->text();
    if (userpassword.isEmpty()) {
        showErrorTip(edit, tr("Password cannot be empty"));
        return false;
    }

    if (m_nameEdit->text().toLower() == userpassword.toLower()) {
        showErrorTip(edit, tr("The password should be different from the username"));
        return false;
    }

    bool result = validatePassword(userpassword);
    if (!result) {
        showErrorTip(edit, tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"));
        return false;
    }

    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->text() != m_repeatpasswdEdit->text()) {
            showErrorTip(m_repeatpasswdEdit, tr("Passwords do not match"));
            return false;
        }
    }

    if (m_errorTip->isVisible()) {
        m_errorTip->hide();
    }
    m_errorEdit = nullptr;
    return true;
}

bool CreateAccountPage::validateUsername(const QString &username)
{
    const QString name_validate = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("-_");
    return containsChar(username, name_validate);
}

bool CreateAccountPage::onNameEditFinished()
{
    QString username = m_nameEdit->text();
    if (username.isEmpty()) {
        showErrorTip(m_nameEdit, tr("Username cannot be empty"));
        return false;
    }

    if (username.size() < 3 || username.size() > 32) {
        showErrorTip(m_nameEdit, tr("Username must be between 3 and 32 characters"));
        return false;
    }

    const QString compStr = "abcdefghijklmnopqrstuvwxyz";
    if (!compStr.contains(username.at(0))) {
        showErrorTip(m_nameEdit, tr("The first character must be in lower case"));
        return false;
    }

    if (!validateUsername(username)) {
        showErrorTip(m_nameEdit, tr("Username must only contain a~z, 0~9, - or _"));
        return false;
    }

    if (m_errorTip->isVisible()) {
        m_errorTip->hide();
    }
    m_errorEdit = nullptr;
    return true;
}
