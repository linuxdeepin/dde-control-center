/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PASSWORDLINE_H
#define PASSWORDLINE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "dseparatorhorizontal.h"
#include "dtextbutton.h"
#include "dheaderline.h"
#include "dlineedit.h"
#include "dconstants.h"
#include "../controlline.h"
#include <QDebug>

DWIDGET_USE_NAMESPACE


class PasswordFrame : public QStackedWidget
{
    Q_OBJECT
public:
    explicit PasswordFrame(QWidget *parent = 0);
    void preDestroy();
    void reset();

signals:
    void sizeChanged();
    void changePassword(QString password);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void onPasswdFocusChanged(bool focus);
    void onPasswdChanged(const QString &);
    void onPasswdRepeatFocusChanged(bool focus);
    void onPasswdRepeatChanged(const QString &passwd);
    void initInactiveWidget();
    void initActiveWidget();
    void resetData();
    bool validate();

private:
    AccountPasswdLine *m_lineNew = NULL;
    AccountPasswdLine *m_lineRepeat = NULL;
    QWidget *m_inactiveWidget = NULL;
};

#endif // PASSWORDLINE_H
