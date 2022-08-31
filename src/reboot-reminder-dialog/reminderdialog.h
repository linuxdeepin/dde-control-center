// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

public Q_SLOTS:
    void Show();

private Q_SLOTS:
    void setupDialog();
    void showDialog();

private:
    QTimer *m_timer;
    com::deepin::SessionManager *m_sessionManager;
    ReminderDialog *m_dialog;
};

#endif // REMINDERDDIALOG_H
