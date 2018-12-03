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

namespace dcc{
namespace update{

DownloadProgressBar::DownloadProgressBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(36);
}

void DownloadProgressBar::setMessage(const QString &message)
{
    m_message = message;

    update();
}

void DownloadProgressBar::setValue(const int progress)
{
    if (m_currentValue == progress) return;

    m_currentValue = progress;

    update();
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

void DownloadProgressBar::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath bg_path;
    bg_path.addRoundedRect(rect(), 5, 5);

    // draw background
    painter.fillPath(bg_path, QColor(0, 0, 0, 0.05 * 255));

    // draw border
    painter.setPen(QColor(255, 255, 255, 0.05 * 255));
    painter.drawPath(bg_path);

    // calculate the area that needs to be drawn
    const QRect &r = rect().adjusted(0, 0, - (width() * (100 - m_currentValue) * 0.01), 0);

    QPainterPath content_path;
    content_path.addRoundedRect(r, 5, 5);

    // draw content
    painter.fillPath(content_path, QColor("#2ca7f8"));
    painter.setPen(QColor("#378cfa"));
    painter.drawPath(content_path);

    // draw text
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, m_message);
}

}
}
