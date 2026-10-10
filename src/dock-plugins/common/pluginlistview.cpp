// SPDX-FileCopyrightText: 2016 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pluginlistview.h"

#include <QDebug>
#include <QScrollBar>
#include <QScroller>
#include <QHoverEvent>

DWIDGET_USE_NAMESPACE

PluginListView::PluginListView(QWidget *parent)
    : DListView(parent)
    , m_delegate(new PluginItemDelegate(this))
{
    setObjectName("DockPluginCommonListView");
    setAccessibleName("DockPluginCommonListView");
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setSelectionMode(QAbstractItemView::NoSelection);
    viewport()->setAutoFillBackground(false);
    setAutoFillBackground(false);
    setAutoScroll(true);
    setDragEnabled(false);
    setViewMode(QListView::ViewMode::ListMode);

    // 支持在触摸屏上滚动
    QScroller::grabGesture(viewport(), QScroller::LeftMouseButtonGesture);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);

    setItemDelegate(m_delegate);
}

void PluginListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
}

int PluginListView::getItemHeight() const
{
    return m_delegate->itemHeight();
}

int PluginListView::getItemSpacing() const
{
    return m_delegate->itemSpacing();
}

void PluginListView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QAbstractItemModel *m = model();
    QList<QModelIndex> indexes;
    indexes.append(parent);
    while (!indexes.isEmpty()) {
        QModelIndex i = indexes.takeFirst();
        if (!isPersistentEditorOpen(i))
            openPersistentEditor(i);
        for (int j = 0; j < m->rowCount(i); j++) {
            indexes.append(m->index(j, 0, i));
        }
    }
    QListView::rowsInserted(parent, start, end);
}

bool PluginListView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverLeave: {
        setCurrentIndex(QModelIndex());
    } break;
    case QEvent::HoverEnter:
    case QEvent::HoverMove: {
        QHoverEvent *he = dynamic_cast<QHoverEvent *>(event);
        QModelIndex newIndex = indexAt(QPoint(he->position().x(), he->position().y()));
        setCurrentIndex(newIndex);
        break;
    }
    default:
        return QListView::viewportEvent(event);
    }
    return true;
}
