/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "reminderdialog.h"

#include <QApplication>
#include <QDesktopWidget>

#include <QDebug>

ReminderDialog::ReminderDialog(QRect screenGeometry)
    : DDialog(tr("Updates installed, need to restart, restart now?"),
              tr("Please make sure all data have been saved firstly")),
      m_screenGeometry(screenGeometry)
{
    QIcon icon( QIcon::fromTheme("system-updated") );
    setIconPixmap(icon.pixmap(48, 48));

    QStringList buttons;
    buttons << tr("Cancel") << tr("Shut down") << tr("Reboot");
    addButtons(buttons);

    setDefaultButton(2);
}

void ReminderDialog::showEvent(QShowEvent *event)
{
    DDialog::showEvent(event);

    move(m_screenGeometry.center() - rect().center());
}

QRect ReminderDialog::screenGeometry() const
{
    return m_screenGeometry;
}

void ReminderDialog::setScreenGeometry(const QRect &screenGeometry)
{
    m_screenGeometry = screenGeometry;
}

Manager::Manager() :
    QObject(),
    m_timer(new QTimer(this)),
    m_sessionManager(new com::deepin::SessionManager("com.deepin.SessionManager",
                                                     "/com/deepin/SessionManager",
                                                     QDBusConnection::sessionBus(), this)),
    m_dialog(nullptr)
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(1000 * 60 * 60 * 2); // 2h

    connect(m_timer, &QTimer::timeout, this, &Manager::showDialog);
}

void Manager::setupDialog()
{
    const QDesktopWidget *desktop = QApplication::desktop();
    const QPoint cursorPoint = QCursor::pos();
    const QRect geom = desktop->screenGeometry(cursorPoint);

    if (!m_dialog)
        m_dialog = new ReminderDialog(geom);
    else
        m_dialog->setScreenGeometry(geom);
}

void Manager::showDialog()
{
    int value = m_dialog->exec();

    if (value == 0) {
        qDebug() << "user dismissed the dialog.";
        m_timer->start();
    } else if (value == 1) {
        qDebug() << "user clicked shutdown";
        m_sessionManager->RequestShutdown();
    } else if (value == 2) {
        qDebug() << "user clicked reboot";
        m_sessionManager->RequestReboot();
    }
}

void Manager::start()
{
    setupDialog();
    showDialog();
}

void Manager::Show()
{
    if (m_dialog->isVisible())
        m_dialog->activateWindow();
    else
        start();
}
