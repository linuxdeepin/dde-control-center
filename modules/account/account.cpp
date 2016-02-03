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

Account::Account()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "User Accounts"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_label = new QLabel("Account");
    m_mainWidget = new AccountMainWidget;
}

Account::~Account()
{
    qDebug() << "~Account()";

    m_label->deleteLater();
    m_mainWidget->deleteLater();
}

QFrame* Account::getContent()
{
    return m_mainWidget;
}

void Account::preUnload()
{
    m_mainWidget->preDestroy();
}
