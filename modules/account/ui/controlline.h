/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONTROLLINE_H
#define CONTROLLINE_H

#include "passwdline.h"
#include "switchline.h"

#include "dlineedit.h"
#include "dtextbutton.h"
#include "dheaderline.h"
#include "dsegmentedcontrol.h"
#include "dseparatorhorizontal.h"

DWIDGET_USE_NAMESPACE

//for special style
class AccountHeaderLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit AccountHeaderLine(QWidget *parent = 0) : DHeaderLine(parent) {D_THEME_INIT_WIDGET(AccountHeaderLine);}
};

class AccountPasswdLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountPasswdLine(QWidget *parent = 0);
    ~AccountPasswdLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DPasswordEdit *passwordEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DPasswordEdit *m_passwordEdit = NULL;
};



class AccountSwitchLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountSwitchLine(QWidget *parent = 0);
    bool check() const;
    void setCheck(bool s);

signals:
    void checkChanged(bool s);

private:
    void setContent(QWidget *content);

private:
    DSwitchButton *m_button = NULL;
};

class AccountConfirmButtonLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountConfirmButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void confirm();
};


class AccountInputLine : public AccountHeaderLine
{
    Q_OBJECT
public:
    explicit AccountInputLine(QWidget *parent = 0);
    ~AccountInputLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DLineEdit *lineEdit() const;

protected:
    void showEvent(QShowEvent *e);
signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    void setContent(QWidget *content);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DLineEdit *m_lineEdit = NULL;
};

#endif // CONTROLLINE_H
