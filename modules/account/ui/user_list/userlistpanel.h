/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ACCOUNTLISTPANEL_H
#define ACCOUNTLISTPANEL_H

#include <QMap>
#include <QList>
#include <QWidget>
#include <QVBoxLayout>
#include "userexpand.h"
#include "dbus/dbussessionmanager.h"
#include "dbus/dbusaccount.h"
#include "libdui/dexpandgroup.h"

class UserListPanel : public QWidget
{
    Q_OBJECT
public:
    explicit UserListPanel(QWidget *parent = 0);
    void preDestroy();

signals:
    void requestPreDestroy();
    void requestDelete(bool flag);
    void cancelDelete();
    void hideForSetting();
    void showForNormal();
    void changeToSetting(bool setting);

private:
    void initSessionManager();
    void onUserAdded(const QString &path);
    void insertToLayout(UserExpand *expand);
    void onUserDeleted(const QString &path);
private slots:
    void initAccount();

private:
    DBusAccount *m_account = NULL;
    QVBoxLayout *m_mainLayout = NULL;
    DBusSessionManager *m_sessionManager = NULL;

    QList<UserExpand *> m_expandList;
};

#endif // ACCOUNTLISTPANEL_H
