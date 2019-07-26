/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "defapplistview.h"

#include <QListView>
#include <QStandardItemModel>
#include <QDebug>
#include <QScroller>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::defapp;

DefAppListView::DefAppListView(QWidget *parent) : QListView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(QListView::ScrollPerPixel);
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
    setUpdatesEnabled(true);

    QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(this);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    connect(this, &DefAppListView::currentHoverChanged, this, &DefAppListView::onCurrentHoverChanged);
    connect(this, &DefAppListView::entered, this, &DefAppListView::onItemEntered);
}

const QModelIndex &DefAppListView::currentHoverIndex() const
{
    return m_indexCurrent;
}

void DefAppListView::enterEvent(QEvent *event)
{
    if (m_indexCurrent.isValid()) {
        openPersistentEditor(m_indexCurrent);
    }

    QWidget::enterEvent(event);
}

void DefAppListView::leaveEvent(QEvent *event)
{
    if (m_indexCurrent.isValid()) {
        closePersistentEditor(m_indexCurrent);
    }
    QWidget::leaveEvent(event);
}

void DefAppListView::onCurrentHoverChanged(const QModelIndex &previous, const QModelIndex &current)
{
    if (previous.isValid()) {
        closePersistentEditor(previous);
    }
    openPersistentEditor(current);
}

void DefAppListView::onItemEntered(const QModelIndex &index)
{
    m_indexCurrent = index;
    if (m_indexPrevious != m_indexCurrent) {
        Q_EMIT currentHoverChanged(m_indexPrevious, m_indexCurrent);
        m_indexPrevious = m_indexCurrent;
    }
}
