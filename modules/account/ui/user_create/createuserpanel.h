/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CREATEUSERPANEL_H
#define CREATEUSERPANEL_H

#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dbus/dbusaccount.h"
#include "dbus/dbusaccountuser.h"
#include "dseparatorhorizontal.h"
#include "useravatar.h"
#include "inputline.h"
#include "../controlline.h"

DWIDGET_USE_NAMESPACE

class CreateUserPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CreateUserPanel(QWidget *parent = 0);
    void preDestroy();

signals:
    void createCancel();
    void createConfirm();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initDbusData();
    void initHeader();
    void initInfoLine();
    void initInputLline();
    void initConfirmLine();

private slots:
    bool validate();
    void resetData();
    void onCancel();
    void onConfirm();
    void onUserAdded(const QString &path);
    void onNameFocusChanged(bool focus);
    void onPasswdFocusChanged(bool focus);
    void onPasswdRepeatFocusChanged(bool focus);
    void onNameChanged(const QString &name);
    void onPasswdChanged(const QString &);
    void onPasswdRepeatChanged(const QString &passwd);
    void onCanHideControlCenter() {this->window()->setProperty("autoHide", true);}

private:
    QString m_oldName = "";
    UserAvatar *m_avatar = NULL;
    DBusAccount *m_account = NULL;
    QVBoxLayout *m_layout = NULL;
    QLabel *m_newNameLabel = NULL;
    AccountInputLine *m_nameLine = NULL;
    AccountSwitchLine *m_autoLogin = NULL;
    AccountPasswdLine *m_passwdNew = NULL;
    AccountPasswdLine *m_passwdRepeat = NULL;
    AccountTypeLine *m_accountType = NULL;
    AccountConfirmButtonLine *m_confirmLine = NULL;

    QString m_randIcon = "";

    const int ICON_SIZE = 60;
};

#endif // CREATEUSERPANEL_H
