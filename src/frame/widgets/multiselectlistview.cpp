#include "widgets/multiselectlistview.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

namespace dcc {
namespace widgets {

MultiSelectListView::MultiSelectListView(QWidget *parent): DListView(parent) {

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

}
}
