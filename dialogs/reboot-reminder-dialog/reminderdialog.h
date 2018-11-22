/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
