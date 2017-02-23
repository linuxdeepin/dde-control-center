/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_ACCOUNTS_CREATEPAGE_H
#define DCC_ACCOUNTS_CREATEPAGE_H

#include <darrowrectangle.h>

#include "contentwidget.h"
#include "avatarwidget.h"
#include "lineeditwidget.h"
#include "settingsgroup.h"
#include "buttontuple.h"

#include "user.h"
#include "creationresult.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {

class ErrorTip : public DArrowRectangle {
    Q_OBJECT
public:
    explicit ErrorTip();

    void setText(QString text);
    void clear();
    bool isEmpty() const;

public slots:
    void appearIfNotEmpty();

private:
    QLabel *m_label;
};

class CreatePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit CreatePage(QWidget *parent = 0);
    ~CreatePage();

    void setModel(User *user);

signals:
    void requestCreateUser(const User *user) const;

public slots:
    void setCreationResult(CreationResult *result);

private:
    void createUser();
    void cancelCreation() const;

    void showUsernameErrorTip(QString error);
    void showPasswordEmptyErrorTip(const QString &error);
    void showPasswordMatchErrorTip(QString error);

private:
    AvatarWidget *m_avatar;
    dcc::widgets::SettingsGroup *m_group;
    dcc::widgets::LineEditWidget *m_username;
    dcc::widgets::LineEditWidget *m_password;
    dcc::widgets::LineEditWidget *m_repeatpass;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    ErrorTip *m_errorTip;

    User *m_user;
};

} // namespace accounts
} // namespace dcc

#endif // DCC_ACCOUNTS_CREATEPAGE_H
