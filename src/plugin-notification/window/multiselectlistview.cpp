//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "multiselectlistview.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

using namespace DCC_NAMESPACE;

MultiSelectListView::MultiSelectListView(QWidget *parent): DListView(parent) {
    setAccessibleName("MultiSelectListView");
    // 禁用横向滚动条,防止内容被截断
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MultiSelectListView::resetStatus(const QModelIndex &index) {
    m_currentIndex = index.row();
    DListView::clearSelection();
    DListView::setSelectionMode(DListView::SingleSelection);
    DListView::setCurrentIndex(index);
}

void MultiSelectListView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    DListView::setSelectionMode(DListView::SingleSelection);
    DListView::currentChanged(current, previous);
}

QModelIndex MultiSelectListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) {
    DListView::setSelectionMode(DListView::MultiSelection);
    return DListView::moveCursor(cursorAction, modifiers);
}

void MultiSelectListView::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Up) {
        QModelIndex nextIndex = model()->index(m_currentIndex - 1, 0);
        if (nextIndex.isValid() ) {
            if (isRowHidden(m_currentIndex - 1)) {
                if ( m_currentIndex -1 <= 0) {
                    return;
                }
                m_currentIndex--;
                keyPressEvent(event);
                return;
            }
            setCurrentIndex(nextIndex);
            Q_EMIT clicked(nextIndex);
        }
        return;
    } else if (event->key() == Qt::Key_Down) {
        QModelIndex nextIndex = model()->index(m_currentIndex + 1, 0);
        if (nextIndex.isValid()) {
            if (isRowHidden(m_currentIndex + 1)) {
                if (model()->rowCount()-1 <= m_currentIndex + 1) {
                    return;
                }
                m_currentIndex++;
                keyPressEvent(event);
                return;
            }
            setCurrentIndex(nextIndex);
            Q_EMIT clicked(nextIndex);
        }
        return;
    }
    return DListView::keyPressEvent(event);
}
