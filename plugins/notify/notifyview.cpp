/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <190771752ri@gmail.com>
 *
 * Maintainer: listenerri <190771752ri@gmail.com>
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

#include "notifyview.h"
#include "notifydelegate.h"

#include <QMouseEvent>

NotifyView::NotifyView(QWidget *parent) : QListView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollMode(QListView::ScrollPerPixel);
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
    setUpdatesEnabled(true);
    setStyleSheet("background-color: rgba(255, 255, 255, 7.65);");

    connect(this, &NotifyView::currentHoverChanged, this, &NotifyView::onCurrentHoverChanged);
}

void NotifyView::onCurrentHoverChanged(const QModelIndex &previous, const QModelIndex &current)
{
    if (previous.isValid()) {
        closePersistentEditor(previous);
    }
    openPersistentEditor(current);
}

void NotifyView::mouseMoveEvent(QMouseEvent *event)
{
    m_indexCurrent = indexAt(event->pos());
    if (m_indexPrevious != m_indexCurrent) {
        Q_EMIT currentHoverChanged(m_indexPrevious, m_indexCurrent);
        m_indexPrevious = m_indexCurrent;
    }

    QListView::mouseMoveEvent(event);
}
