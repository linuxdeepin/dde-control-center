/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "createpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "translucentframe.h"

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
    layout->setSpacing(10);

    setContent(container);
    setTitle(tr("New Account"));

    connect(confirmBtn, &QPushButton::clicked, this, &CreatePage::createUser);
    connect(cancelBtn, &QPushButton::clicked, this, &CreatePage::cancelCreation);

    connect(this, &CreatePage::back, m_errorTip, &ErrorTip::hide);
    connect(this, &CreatePage::disappear, m_errorTip, &ErrorTip::hide);
    connect(this, &CreatePage::appear, m_errorTip, &ErrorTip::appearIfNotEmpty, Qt::QueuedConnection);

    connect(m_username->textEdit(), &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_username->textEdit(), &QLineEdit::textEdited, [=] { m_username->textEdit()->setText(m_username->textEdit()->text().toLower()); });
    connect(m_repeatpass->textEdit(), &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
    connect(m_password->textEdit(), &QLineEdit::textChanged, m_errorTip, &ErrorTip::hide);
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
        emit back();
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
        emit back();
        break;
    default:
        break;
    }

    result->deleteLater();
}

void CreatePage::createUser()
{
    if (m_username->text().isEmpty())
        return showUsernameErrorTip(tr("Username can't be empty."));

    if (m_password->text().isEmpty())
        return showPasswordEmptyErrorTip(tr("Password can't be empty."));

    m_user->setName(m_username->text());
    m_user->setPassword(m_password->text());
    m_user->setRepeatPassword(m_repeatpass->text());

    emit requestCreateUser(m_user);
}

void CreatePage::cancelCreation() const
{
    m_errorTip->hide();

    emit back();
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

ErrorTip::ErrorTip() :
    DArrowRectangle(DArrowRectangle::ArrowTop),
    m_label(new QLabel)
{
    m_label->setObjectName("New_Account_errorTip");
    m_label->setAccessibleName(m_label->objectName());
    m_label->setStyleSheet("padding: 5px 10px; color: #f9704f");
    setContent(m_label);
}

void ErrorTip::setText(QString text)
{
    m_label->setText(text);
    m_label->setAccessibleDescription(text);
    m_label->adjustSize();
    resizeWithContent();
}

void ErrorTip::clear()
{
    m_label->clear();
    hide();
}

bool ErrorTip::isEmpty() const
{
    return m_label->text().isEmpty();
}

void ErrorTip::appearIfNotEmpty()
{
    if (!isEmpty() && !isVisible())
        QWidget::show();
}

} // namespace accounts
} // namespace dcc
