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

#include "reminderdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include <QDebug>

ReminderDialog::ReminderDialog(QRect screenGeometry)
    : DDialog(tr("Updates installed! A restart is required, restart now?"),
              tr("Make sure all data has been saved before proceeding")),
      m_screenGeometry(screenGeometry)
{
    const qreal ratio = devicePixelRatioF();
    QPixmap icon(QIcon::fromTheme("system-updated").pixmap(QSize(48, 48) * ratio));
    icon.setDevicePixelRatio(ratio);
    setIconPixmap(icon);

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
