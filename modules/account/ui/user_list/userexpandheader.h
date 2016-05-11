/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef USEREXPANDHEADER_H
#define USEREXPANDHEADER_H

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include "useravatar.h"
#include "usernametitle.h"
#include "dbus/dbusaccount.h"
#include "dbus/dbusaccountuser.h"
#include "darrowbutton.h"
#include "dsegmentedcontrol.h"
#include "confirmbuttonline.h"
#include "multideletebutton.h"

DWIDGET_USE_NAMESPACE

class DeleteButton : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setFixedWidth)

public:
    DeleteButton(QWidget *parent = 0);
    void showIcon();
    void hideIcon();

signals:
    void clicked();

private:
    QPropertyAnimation *m_animation = NULL;
    MultiDeleteButton *m_deleteButton = NULL;

    const int ICON_WIDTH = 52;
    const int ANIMATION_DURATION = 100;
    const QEasingCurve ANIMATION_CURVE = QEasingCurve::OutQuad;
};

class UserExpandHeader : public QLabel
{
    Q_OBJECT
public:
    explicit UserExpandHeader(const QString &userPath, QWidget *parent = 0);
    void setIsCurrentUser(bool isCurrentUser);
    void setExpand(bool value);
    void changeToDeleteState(bool value);
    bool userLocked() const;
    QString userName() const;

signals:
    void mousePress();
    void cancelDelete();
    void lockChanged();

protected:
    void mousePressEvent(QMouseEvent *);

private slots:
    void onCanHideControlCenter() {this->window()->setProperty("autoHide", true);}

private:
    void initData();
    void initIcon();
    void initRightStack();
    void initDeleteButton();
    void reverseArrowDirection();
    QString getTypeName(int type);

    void updateIcon();
    void updateAccountName();

    void onCancelDeleteUser();
    void onConfirmDeleteUser();

private:
    QString m_userPath = "";
    UserAvatar *m_icon = NULL;
    QHBoxLayout *m_mainLayout = NULL;
    UserNameTitle *m_nameTitle = NULL;
    DBusAccountUser *m_accountUser = NULL;
    DSegmentedControl *m_folderControl = NULL;
    DArrowButton *m_arrowButton = NULL;
    QStackedWidget *m_rightStack = NULL;
    DeleteButton *m_deleteButton = NULL;
    bool m_adminCurrentLogin = true;
    bool m_isCurrentUser = false;
    bool m_inDeleteState = false;

    const int ICON_WIDTH = 118;
    const int ICON_NORMAL_HEIGHT = 100;
    const int ICON_BIG_HEIGHT = 118;
};

#endif // USEREXPANDHEADER_H
