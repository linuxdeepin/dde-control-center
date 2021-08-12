/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "downloadprogressbar.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>

using namespace dcc::update;

DownloadProgressBar::DownloadProgressBar(QWidget* parent)
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
    Q_EMIT clicked();

    QWidget::mouseReleaseEvent(e);
}
