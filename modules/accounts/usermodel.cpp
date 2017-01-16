/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "usermodel.h"

#include <QDebug>

using namespace dcc::accounts;

UserModel::UserModel(QObject *parent) :
    QObject(parent)
{

}

UserModel::~UserModel()
{

}

User * UserModel::getUser(const QString &id)
{
    return m_userList.value(id, nullptr);
}

QList<User *> UserModel::userList() const
{
    return m_userList.values();
}

void UserModel::addUser(const QString &id, User *user)
{
    Q_ASSERT(!m_userList.contains(id));

    m_userList[id] = user;

    emit userAdded(user);
}

void UserModel::removeUser(const QString &id)
{
    User * user = getUser(id);
    m_userList.remove(id);

    if (user)
        emit userRemoved(user);
}

bool UserModel::contains(const QString &id)
{
    return m_userList.contains(id);
}
