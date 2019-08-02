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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QMap>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout)
    , m_inputLayout(new QVBoxLayout)
    , m_selectLayout(new QHBoxLayout)
    , m_nameLayout(new QVBoxLayout)
    , m_fullnameLayout(new QVBoxLayout)
    , m_passwdLayout(new QVBoxLayout)
    , m_repeatpasswdLayout(new QVBoxLayout)
    , m_passwdbtnLayout(new QHBoxLayout)
    , m_repeatpasswdbtnLayout(new QHBoxLayout)
    , m_avatarListWidget(new AvatarListWidget)
    , m_nameLabel(new QLabel)
    , m_fullnameLabel(new QLabel)
    , m_passwdLabel(new QLabel)
    , m_repeatpasswdLabel(new QLabel)
    , m_nameEdit(new QLineEdit)
    , m_fullnameEdit(new QLineEdit)
    , m_passwdEdit(new QLineEdit)
    , m_repeatpasswdEdit(new QLineEdit)
    , m_cancleBtn(new QPushButton)
    , m_addBtn(new QPushButton)
    , m_errorTip(new ErrorTip)
    , m_passwdBtn(new DImageButton)
    , m_repeatpasswdBtn(new DImageButton)
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
    m_passwdbtnLayout->setMargin(0);
    m_passwdbtnLayout->setSpacing(5);

    m_repeatpasswdbtnLayout->setMargin(0);
    m_repeatpasswdbtnLayout->setSpacing(5);

    m_mainContentLayout->addWidget(m_avatarListWidget);
    m_mainContentLayout->addLayout(m_inputLayout);
    m_mainContentLayout->addLayout(m_selectLayout);
    m_mainContentLayout->addStretch();

    m_inputLayout->addLayout(m_nameLayout);
    m_inputLayout->addLayout(m_fullnameLayout);
    m_inputLayout->addLayout(m_passwdLayout);
    m_inputLayout->addLayout(m_repeatpasswdLayout);

    m_selectLayout->addWidget(m_cancleBtn, 0, Qt::AlignCenter);
    m_selectLayout->addWidget(m_addBtn, 0, Qt::AlignCenter);

    m_nameLayout->addWidget(m_nameLabel);
    m_nameLayout->addWidget(m_nameEdit);

    m_fullnameLayout->addWidget(m_fullnameLabel);
    m_fullnameLayout->addWidget(m_fullnameEdit);

    m_passwdbtnLayout->addWidget(m_passwdEdit);
    m_passwdbtnLayout->addWidget(m_passwdBtn);

    m_repeatpasswdbtnLayout->addWidget(m_repeatpasswdEdit);
    m_repeatpasswdbtnLayout->addWidget(m_repeatpasswdBtn);

    m_passwdLayout->addWidget(m_passwdLabel);
    m_passwdLayout->addLayout(m_passwdbtnLayout);

    m_repeatpasswdLayout->addWidget(m_repeatpasswdLabel);
    m_repeatpasswdLayout->addLayout(m_repeatpasswdbtnLayout);

    setLayout(m_mainContentLayout);

    m_editButtonMap.insert(m_passwdBtn, m_passwdEdit);
    m_editButtonMap.insert(m_repeatpasswdBtn, m_repeatpasswdEdit);

    m_errorTip->setWindowFlags(Qt::ToolTip);
    m_errorTip->hide();
}

void CreateAccountPage::initDatas()
{
    for (auto it = m_editButtonMap.begin(); it != m_editButtonMap.end(); it++) {
        connect(it.key(), &DImageButton::clicked, this, &CreateAccountPage::updateLineEditDisplayStyle, Qt::QueuedConnection);
        Q_EMIT it.key()->clicked();
    }

    connect(m_cancleBtn, &QPushButton::clicked, this, &CreateAccountPage::requestBack);
    connect(m_addBtn, &QPushButton::clicked, this, &CreateAccountPage::createUser);
    connect(m_nameEdit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_fullnameEdit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_passwdEdit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_repeatpasswdEdit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);

    connect(m_passwdEdit, &QLineEdit::editingFinished, this, [ = ] {
        onEditFinished<QLineEdit *>(m_passwdEdit);
    });

    connect(m_repeatpasswdEdit, &QLineEdit::editingFinished, this, [ = ] {
        if (!m_errorTip->isVisible())
        {
            onEditFinished<QLineEdit *>(m_repeatpasswdEdit);
        }
    });

    m_nameLabel->setText("账户名");
    m_nameEdit->setPlaceholderText("必填");
    m_fullnameLabel->setText("全名");
    m_fullnameEdit->setPlaceholderText("必填");
    m_passwdLabel->setText("密码");
    m_passwdEdit->setPlaceholderText("必填");
    m_repeatpasswdLabel->setText("重复密码");
    m_repeatpasswdEdit->setPlaceholderText("必填");

    m_cancleBtn->setText("取消");
    m_addBtn->setText("添加");
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
        showUsernameErrorTip(tr("用户名不能为空"));
        return;
    }
    if (m_passwdEdit->text().isEmpty()) {
        showPasswordEmptyErrorTip(tr("密码不能为空"));
        return;
    }
    if (m_passwdEdit->text() != m_repeatpasswdEdit->text()) {
        showPasswordMatchErrorTip(tr("2次输入的密码不一致"));
        return;
    }

    m_newUser->setName(m_nameEdit->text());
    m_newUser->setFullname(m_fullnameEdit->text());
    m_newUser->setPassword(m_passwdEdit->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->text());

    Q_EMIT requestCreateUser(m_newUser);
}

bool CreateAccountPage::validatePassword(const QString &password)
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

bool CreateAccountPage::containsChar(const QString &password, const QString &validate)
{
    for (const QString &p : password) {
        if (validate.contains(p)) {
            return true;
        }
    }

    return false;
}

void CreateAccountPage::showUsernameErrorTip(const QString &error)
{
    QPoint globalStart = m_nameEdit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_nameEdit->width() / 2,
                     globalStart.y() + m_nameEdit->height() / 2 + 10);
}

void CreateAccountPage::showUserfullnameErrorTip(const QString &error)
{
    QPoint globalStart = m_fullnameEdit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_fullnameEdit->width() / 2,
                     globalStart.y() + m_fullnameEdit->height() / 2 + 10);
}

void CreateAccountPage::showPasswordEmptyErrorTip(const QString &error)
{
    QPoint globalStart = m_passwdEdit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_passwdEdit->width() / 2,
                     globalStart.y() + m_passwdEdit->height() / 2 + 10);
}

void CreateAccountPage::showPasswordMatchErrorTip(const QString &error)
{
    QPoint globalStart = m_repeatpasswdEdit->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_repeatpasswdEdit->width() / 2,
                     globalStart.y() + m_repeatpasswdEdit->height() / 2 + 10);
}

void CreateAccountPage::updateLineEditDisplayStyle()
{
    DImageButton* button = qobject_cast<DImageButton*>(sender());
    QLineEdit* edit = m_editButtonMap[button];
    const bool isPasswordMode = edit->echoMode() == QLineEdit::Password;

    edit->setEchoMode(isPasswordMode ? QLineEdit::Normal : QLineEdit::Password);

    button->setNormalPic(QString(":/widgets/themes/dark/icons/password_%1_normal.svg").arg(isPasswordMode ? "show" : "hide"));
    button->setHoverPic(QString(":/widgets/themes/dark/icons/password_%1_hover.svg").arg(isPasswordMode ? "show" : "hide"));
    button->setPressPic(QString(":/widgets/themes/dark/icons/password_%1_press.svg").arg(isPasswordMode ? "show" : "hide"));
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    qDebug() << Q_FUNC_INFO << "result->type()=" << result->type();
    switch (result->type()) {
    case CreationResult::NoError:
        break;
    case CreationResult::UserNameError:
        showUsernameErrorTip(result->message());
        break;
    case CreationResult::PasswordMatchError:
        showPasswordMatchErrorTip(result->message());
        break;
    case CreationResult::PasswordError:
        showPasswordEmptyErrorTip(result->message());
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        break;
    default:
        break;
    }

    result->deleteLater();
}

template <typename T>
void CreateAccountPage::onEditFinished(T t)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    if (!setting.value("STRONG_PASSWORD", false).toBool()) {
        return m_errorTip->hide();
    }

    const QString &password = t->text();
    if (m_nameEdit->text().toLower() == password.toLower()) {
        showPasswordEmptyErrorTip(tr("The password should be different from the username"));
        return;
    }

    if (!validatePassword(password)) {
        if (t == m_passwdEdit) {
            showPasswordEmptyErrorTip(tr("The password must contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
        } else {
            showPasswordMatchErrorTip(tr("The password must contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
        }
    } else {
        m_errorTip->hide();
    }
}
