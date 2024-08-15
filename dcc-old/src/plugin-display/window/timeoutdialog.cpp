//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "timeoutdialog.h"
#include <DIconTheme>
DGUI_USE_NAMESPACE

TimeoutDialog::TimeoutDialog(const int timeout, QString messageModel, QWidget *parent)
    : DDialog(parent)
    , m_timeoutRefreshTimer(new QTimer(this))
    , m_timeout(timeout)
    , m_messageModel(messageModel)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | (windowFlags()&~Qt::WindowMinMaxButtonsHint));
    // set default title, message icon
    setTitle(tr("Save the display settings?"));
    if (messageModel.isEmpty()) {
        m_messageModel = tr("Settings will be reverted in %1s.");
    }
    setMessage(m_messageModel.arg(m_timeout));
    setIcon(DIconTheme::findQIcon("preferences-system"));

    addButton(tr("Revert"), true, DDialog::ButtonRecommend);
    addButton(tr("Save"));

    m_timeoutRefreshTimer->setInterval(1000);

    connect(m_timeoutRefreshTimer, &QTimer::timeout, this, &TimeoutDialog::onRefreshTimeout);
    setAttribute(Qt::WA_DeleteOnClose);
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

void TimeoutDialog::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return;
}
