/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QDebug>

#include "account.h"
#include "ui/accountmainwidget.h"

Account::Account()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "User Accounts"));
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);
    mainWidget = new AccountMainWidget;
}

Account::~Account()
{
    qDebug() << "~Account()";
}

QFrame *Account::getContent()
{
    return mainWidget;
}

void Account::preUnload()
{
    mainWidget->preDestroy();
}

AccountModule::AccountModule()
{
    account = NULL;
    qDebug() << "AccountModule" << "load";
}

AccountModule::~AccountModule()
{
    qDebug() << "AccountModule" << "destroy";
}

QFrame *AccountModule::getContent()
{
    if (!account) {
        account = new Account;
    }
    return account->getContent();
}

void AccountModule::preUnload()
{
    account->preUnload();
}
