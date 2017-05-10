/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef REMINDERDDIALOG_H
#define REMINDERDDIALOG_H

#include <QTimer>

#include <ddialog.h>

#include <com_deepin_sessionmanager.h>


DWIDGET_USE_NAMESPACE

class ReminderDialog : public DDialog
{
    Q_OBJECT
public:
    explicit ReminderDialog(QRect screenGeometry);
    ~ReminderDialog() {}

    QRect screenGeometry() const;
    void setScreenGeometry(const QRect &screenGeometry);

protected:
    void showEvent(QShowEvent *event);

private:
    QRect m_screenGeometry;
};

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager();
    ~Manager() {}

    void start();

public slots:
    void Show();

private slots:
    void setupDialog();
    void showDialog();

private:
    QTimer *m_timer;
    com::deepin::SessionManager *m_sessionManager;
    ReminderDialog *m_dialog;
};

#endif // REMINDERDDIALOG_H
