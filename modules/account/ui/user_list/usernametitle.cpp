/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "usernametitle.h"

const int MAX_NAME_LENGTH = 16;
UserNameTitle::UserNameTitle(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_nameLabel = new HeaderNameLabel(this);
    m_nameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_typeLabel = new QLabel(this);
    m_typeLabel->setObjectName("HeaderTypeLabel");
    m_typeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_mainLayout->addWidget(m_nameLabel);
    m_mainLayout->addWidget(m_typeLabel);
}

void UserNameTitle::setUserName(const QString &name)
{
    QString tmpName = name.length() > MAX_NAME_LENGTH ? (name.left(MAX_NAME_LENGTH) + "...") : name;
    m_nameLabel->setText(tmpName);
}

void UserNameTitle::setUserType(const QString &type)
{
    m_typeLabel->setText(type);
}

void UserNameTitle::setIsCurrentUser(bool v)
{
    m_nameLabel->setIsCurrentUser(v);
}


HeaderNameLabel::HeaderNameLabel(QWidget *)
{

}

void HeaderNameLabel::setIsCurrentUser(bool value)
{
    m_isCurrentUser = value;
}
bool HeaderNameLabel::isCurrentUser() const
{
    return m_isCurrentUser;
}

