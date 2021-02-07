/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "widgets/timeoutdialog.h"

TimeoutDialog::TimeoutDialog(const int timeout, QString messageModel, QWidget *parent)
    : DDialog(parent)
    , m_timeoutRefreshTimer(new QTimer(this))
    , m_timeout(timeout)
    , m_messageModel(messageModel)
{
    // set default title, message icon
    setTitle(tr("Do you want to save the display settings?"));
    if (messageModel.isEmpty()) {
        m_messageModel = tr("If no operation, the display will be restored after %1s.");
    }
    setMessage(m_messageModel.arg(m_timeout));
    setIcon(QIcon::fromTheme("preferences-system"));

    addButton(tr("Restore"), true, DDialog::ButtonRecommend);
    addButton(tr("Save"));

    m_timeoutRefreshTimer->setInterval(1000);

    connect(m_timeoutRefreshTimer, &QTimer::timeout, this, &TimeoutDialog::onRefreshTimeout);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint);
}

int TimeoutDialog::exec()
{
    m_timeoutRefreshTimer->start();

    return DDialog::exec();
}

void TimeoutDialog::open()
{
    if (!isVisible()) {
        m_timeoutRefreshTimer->start();
    }

    DDialog::open();
}

void TimeoutDialog::onRefreshTimeout()
{
    m_timeout--;
    setMessage(m_messageModel.arg(m_timeout));

    if (m_timeout < 1) {
        reject();
    }
}

QString TimeoutDialog::messageModel() const
{
    return m_messageModel;
}

void TimeoutDialog::setMessageModel(const QString &messageModel)
{
    m_messageModel = messageModel;
    setMessage(m_messageModel.arg(m_timeout));
}
