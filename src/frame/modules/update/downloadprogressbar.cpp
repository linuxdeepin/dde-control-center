// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "downloadprogressbar.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>

using namespace dcc::update;

DownloadProgressBar::DownloadProgressBar(QWidget *parent)
    : QProgressBar(parent)
    , m_currentValue(0)
{
    setAccessibleName("DownloadProgressBar");
    setFixedHeight(36);
    setTextVisible(true);
    setTextDirection(QProgressBar::TopToBottom);
    setRange(0, 100);
    setAlignment(Qt::AlignCenter);
}

void DownloadProgressBar::setMessage(const QString &message)
{
    setFormat(message);
    setValue(m_currentValue);
}

void DownloadProgressBar::setProcessValue(const int progress)
{
    if (m_currentValue == progress || progress < minimum() || progress > maximum())
        return;

    m_currentValue = progress;

    setValue(progress);
}

void DownloadProgressBar::mouseReleaseEvent(QMouseEvent *e)
{
    if (!isEnabled()) {
        return;
    }

    e->accept();
    if (e->button() == Qt::LeftButton) {
        Q_EMIT clicked();
    }

    QWidget::mouseReleaseEvent(e);
}
