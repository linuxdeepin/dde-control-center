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
#include "widgets/titlelabel.h"

#include <DFontSizeManager>

#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget(parent)
    , m_newUser{nullptr}
    , m_avatarListWidget(new AvatarListWidget(m_newUser, this))
    , m_nameEdit(new DLineEdit)
    , m_fullnameEdit(new DLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainContentLayout->setMargin(0);
    setLayout(mainContentLayout);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(scrollArea);

    auto contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    auto tw = new QWidget();
    tw->setLayout(contentLayout);
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);
    scrollArea->setWidget(tw);

    initWidgets(contentLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *addBtn = new DSuggestButton(tr("Create"));
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(addBtn);
    mainContentLayout->addLayout(btnLayout);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });
    connect(addBtn, &DSuggestButton::clicked, this, &CreateAccountPage::createUser);

}

CreateAccountPage::~CreateAccountPage()
{
}

void CreateAccountPage::initWidgets(QVBoxLayout *layout)
{
    //~ contents_path /accounts/New Account
    TitleLabel *titleLabel = new TitleLabel(tr("New Account"));
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addSpacing(17);
    layout->addWidget(titleLabel);

    m_avatarListWidget->setAvatarSize(QSize(40, 40));
    m_avatarListWidget->setViewportMargins(0, 0, 0, 0);
    m_avatarListWidget->setSpacing(14);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addSpacing(10);
    layout->addWidget(m_avatarListWidget, 0, Qt::AlignTop);

    QLabel *nameLabel = new QLabel(tr("Username") + ':');
    layout->addWidget(nameLabel);
    layout->addWidget(m_nameEdit);
    layout->addSpacing(10);

    QLabel *fullnameLabel = new QLabel(tr("Full Name") + ':');
    layout->addWidget(fullnameLabel);
    layout->addWidget(m_fullnameEdit);
    layout->addSpacing(10);

    QLabel *passwdLabel = new QLabel(tr("Password") + ':');
    layout->addWidget(passwdLabel);
    layout->addWidget(m_passwdEdit);
    layout->addSpacing(10);

    QLabel *repeatpasswdLabel = new QLabel(tr("Repeat Password") + ':');
    layout->addWidget(repeatpasswdLabel);
    layout->addWidget(m_repeatpasswdEdit);

    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar,
            m_avatarListWidget, &AvatarListWidget::setCurrentAvatarChecked);
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar,
    this, [this](dcc::accounts::User *user, const QString &file) {
        Q_UNUSED(user)
        m_avatarListWidget->setCurrentAvatarChecked(file);
    });

    connect(m_nameEdit, &DLineEdit::textEdited, this, [ = ](const QString &str) {
        if (m_nameEdit->isAlert()) {
            m_nameEdit->hideAlertMessage();
            m_nameEdit->setAlert(false);
        }
        m_nameEdit->lineEdit()->blockSignals(true);
        auto idx = m_nameEdit->lineEdit()->cursorPosition();
        m_nameEdit->lineEdit()->setText(str.toLower());
        m_nameEdit->lineEdit()->setCursorPosition(idx);
        m_nameEdit->lineEdit()->blockSignals(false);
    });

    connect(m_fullnameEdit, &DLineEdit::textEdited, this, [ = ] {
        if (m_fullnameEdit->isAlert()){
            m_fullnameEdit->hideAlertMessage();
            m_fullnameEdit->setAlert(false);
        }
    });

    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit->isAlert()) {
            m_passwdEdit->hideAlertMessage();
            m_passwdEdit->setAlert(false);
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
            m_repeatpasswdEdit->setAlert(false);
        }
    });

    m_nameEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_fullnameEdit->lineEdit()->setPlaceholderText(tr("optional"));//选填
    m_passwdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_repeatpasswdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
}

void CreateAccountPage::setModel(User *user)
{
    m_newUser = user;
    Q_ASSERT(m_newUser);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!onNameEditFinished(m_nameEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_passwdEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_repeatpasswdEdit)) {
        return;
    }

    //如果用户没有选图像
    auto avatarPaht = m_avatarListWidget->getAvatarPath();
    m_newUser->setCurrentAvatar(avatarPaht);
    m_newUser->setName(m_nameEdit->lineEdit()->text());
    m_newUser->setFullname(m_fullnameEdit->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());

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

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        Q_EMIT requestBack(AccountsWidget::CreateUserSuccess);
        break;
    case CreationResult::UserNameError:
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(result->message(), -1);
        break;
    case CreationResult::PasswordError:
        m_passwdEdit->setAlert(true);
        m_passwdEdit->showAlertMessage(result->message(), -1);
        break;
    case CreationResult::PasswordMatchError:
        m_repeatpasswdEdit->setAlert(true);
        m_repeatpasswdEdit->showAlertMessage(result->message(), -1);
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        break;
    }

    result->deleteLater();
}

bool CreateAccountPage::onPasswordEditFinished(DPasswordEdit *edit)
{
    const QString &userpassword = edit->lineEdit()->text();
    if (userpassword.isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), -1);
        return false;
    }

//    if (m_nameEdit->text().toLower() == userpassword.toLower()) {
//        edit->setAlert(true);
//        edit->showAlertMessage(tr("The password should be different from the username"), -1);
//        return false;
//    }

    bool result = validatePassword(userpassword);
    if (!result) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"), -1);
        return false;
    }

    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->setAlert(true);
            m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"), -1);
            return false;
        }
    }
    return true;
}

bool CreateAccountPage::validateUsername(const QString &username)
{
    const QString name_validate = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("-_");
    return containsChar(username, name_validate);
}

bool CreateAccountPage::onNameEditFinished(DLineEdit *edit)
{
    const QString &username = edit->lineEdit()->text();
    if (username.isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Username cannot be empty"), -1);
        return false;
    }

    if (username.size() < 3 || username.size() > 32) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Username must be between 3 and 32 characters"), -1);
        return false;
    }

    const QString compStr = "abcdefghijklmnopqrstuvwxyz";
    if (!compStr.contains(username.at(0))) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("The first character must be in lower case"), -1);
        return false;
    }

    if (!validateUsername(username)) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Username must only contain a~z, 0~9, - or _"), -1);
        return false;
    }
    return true;
}
