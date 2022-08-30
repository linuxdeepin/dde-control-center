// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "reminderdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include <QDebug>

ReminderDialog::ReminderDialog(QRect screenGeometry)
    : DDialog(tr("Restart the computer to use the system and the applications properly"),
              tr("Make sure all data had been saved first")),
      m_screenGeometry(screenGeometry)
{
    setIcon(QIcon::fromTheme("system-updated"));
    QStringList buttons;
    buttons << tr("Cancel") << tr("Shut down");
    addButtons(buttons);

    addButton(tr("Reboot"), true, ButtonRecommend);
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
    const auto ratio = qApp->devicePixelRatio();
    const QPoint cp(QCursor::pos());

    for (const auto *s : qApp->screens()) {
        const QRect &g = s->geometry();
        const QRect realRect(g.topLeft() / ratio, g.size());

        // Without using the processed geometry and mouse coordinates,
        // the results can be obtained using the original information.
        // If the original screen contains the original mouse, save the scaled geometry.
        if (g.contains(cp)) {
            if (!m_dialog)
                m_dialog = new ReminderDialog(realRect);
            else
                m_dialog->setScreenGeometry(realRect);

            break;
        }
    }
}

void Manager::showDialog()
{
    int value = m_dialog->exec();
    switch (value) {
    case -1:
        qDebug() << "user closed the dialog.";
        m_timer->start();
        break;
    case 0:
        qDebug() << "user dismissed the dialog.";
        m_timer->start();
        break;
    case 1:
        qDebug() << "user clicked shutdown";
        m_sessionManager->RequestShutdown();
        break;
    case 2:
        qDebug() << "user clicked reboot";
        m_sessionManager->RequestReboot();
        break;
    default:
        qDebug() << "reminder dialog error!";
        break;
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
