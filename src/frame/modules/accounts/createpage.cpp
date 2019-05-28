/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "createpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QSettings>
#include <QApplication>

#include "widgets/translucentframe.h"

using namespace dcc::widgets;

namespace dcc {
namespace accounts {

CreatePage::CreatePage(QWidget *parent) :
    ContentWidget(parent),
    m_avatar(new AvatarWidget),
    m_group(new SettingsGroup),
    m_username(new LineEditWidget),
    m_password(new LineEditWidget),
    m_repeatpass(new LineEditWidget),
    m_buttonTuple(new ButtonTuple),
    m_errorTip(new ErrorTip)
{
    m_avatar->setFixedSize(90, 90);

    m_username->setTitle(tr("Username"));
    m_username->setPlaceholderText(tr("Required"));
    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Required"));
    m_password->textEdit()->setEchoMode(QLineEdit::Password);
    m_repeatpass->setTitle(tr("Repeat password"));
    m_repeatpass->setPlaceholderText(tr("Required"));
    m_repeatpass->textEdit()->setEchoMode(QLineEdit::Password);

    m_errorTip->setWindowFlags(Qt::ToolTip);
    m_errorTip->hide();

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *confirmBtn = m_buttonTuple->rightButton();
    cancelBtn->setText(tr("Cancel"));
    confirmBtn->setText(tr("Create"));

    cancelBtn->setAccessibleName("New_Account_Cancel");
    confirmBtn->setAccessibleName("New_Account_Create");

    m_group->appendItem(m_username);
    m_group->appendItem(m_password);
    m_group->appendItem(m_repeatpass);

    TranslucentFrame *container = new TranslucentFrame;

    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->addSpacing(10);
    layout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    layout->addWidget(m_group);
    layout->addWidget(m_buttonTuple);
    layout->addStretch();
    layout->setSpacing(10);

    setContent(container);
    setTitle(tr("New Account"));

    m_username->textEdit()->installEventFilter(this);

    connect(confirmBtn, &QPushButton::clicked, this, &CreatePage::createUser);
    connect(cancelBtn, &QPushButton::clicked, this, &CreatePage::cancelCreation);

    connect(this, &CreatePage::back, m_errorTip, &ErrorTip::hide);
    connect(this, &CreatePage::disappear, m_errorTip, &ErrorTip::hide);
    connect(this, &CreatePage::appear, m_errorTip, &ErrorTip::appearIfNotEmpty, Qt::QueuedConnection);

    connect(m_username->textEdit(), &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, [=] {
        onEditFinished<LineEditWidget*>(m_password);
    });
    connect(m_repeatpass->textEdit(), &QLineEdit::editingFinished, this, [=] {
        if (!m_errorTip->isVisible()) {
            onEditFinished<LineEditWidget*>(m_repeatpass);
        }
    });

    QList<QLineEdit*> list {
        m_username->textEdit(),
        m_password->textEdit(),
        m_repeatpass->textEdit()
    };

    for (QLineEdit* edit : list) {
        connect(edit, &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    }
}

CreatePage::~CreatePage()
{
    m_errorTip->deleteLater();
}

void CreatePage::setModel(User *user)
{
    m_user = user;

    connect(user, &User::currentAvatarChanged, m_avatar, &AvatarWidget::setAvatarPath);
}

void CreatePage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        Q_EMIT back();
        break;
    case CreationResult::UserNameError:
        showUsernameErrorTip(result->message());
        break;
    case CreationResult::PasswordMatchError:
        showPasswordMatchErrorTip(result->message());
        break;
    case CreationResult::PasswordError:
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        Q_EMIT back();
        break;
    default:
        break;
    }

    result->deleteLater();
}

void CreatePage::createUser()
{
    if (m_username->text().isEmpty())
        return showUsernameErrorTip(tr("Username can't be empty"));

    if (m_password->text().isEmpty())
        return showPasswordEmptyErrorTip(tr("Password can't be empty"));

    m_user->setName(m_username->text());
    m_user->setPassword(m_password->text());
    m_user->setRepeatPassword(m_repeatpass->text());

    Q_EMIT requestCreateUser(m_user);
}

void CreatePage::cancelCreation() const
{
    m_errorTip->hide();

    Q_EMIT back();
}

void CreatePage::showUsernameErrorTip(QString error)
{
    QPoint globalStart = m_username->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_username->width() / 2,
                     globalStart.y() + m_username->height() / 2 + 10);
}

void CreatePage::showPasswordEmptyErrorTip(const QString &error)
{
    QPoint globalStart = m_password->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_password->width() / 2,
                     globalStart.y() + m_password->height() / 2 + 10);
}

void CreatePage::showPasswordMatchErrorTip(QString error)
{
    QPoint globalStart = m_repeatpass->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_repeatpass->width() / 2,
                     globalStart.y() + m_repeatpass->height() / 2 + 10);
}

bool CreatePage::ContainsChar(const QString &password, const QString &validate)
{
    for (const QString &p : password) {
        if (validate.contains(p)) {
            return true;
        }
    }

    return false;
}

class FakeKeyEvent : public QKeyEvent {
// The meaning of existence is to modify the text to lowercase
    friend class CreatePage;
};
bool CreatePage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_username->textEdit() && event->type() == QEvent::KeyPress) {
        FakeKeyEvent *tmp_event = static_cast<FakeKeyEvent*>(event);
        tmp_event->txt = tmp_event->txt.toLower();
    }

    return false;
}

bool CreatePage::validatePassword(const QString &password)
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

template <typename T>
void CreatePage::onEditFinished(T t)
{
    QSettings setting("/etc/deepin/dde-control-center.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    if (!setting.value("STRONG_PASSWORD", false).toBool()) return m_errorTip->hide();

    const QString &password = t->text();
    if (m_username->text().toLower() == password.toLower()) {
        showPasswordEmptyErrorTip(tr("The password should be different from the username"));
        return;
    }

    if (!validatePassword(password)) {
        if (t == m_password) {
            showPasswordEmptyErrorTip(tr("The password must contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
        }
        else {
            showPasswordMatchErrorTip(tr("The password must contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
        }
    }
    else {
        m_errorTip->hide();
    }
}

} // namespace accounts
} // namespace dcc
