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

void MultiSelectListView::mouseMoveEvent(QMouseEvent *event)
{
    return ;
}

void MultiSelectListView::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex currentIndex = DListView::currentIndex();
    if (currentIndex.isValid() && (currentIndex.row() >= 0)) {
        setCurrentIndex(currentIndex);
        Q_EMIT clicked(currentIndex);
    }
    return DListView::mouseReleaseEvent(event);
}

void MultiSelectListView::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Up) {
        QModelIndex nextIndex = model()->index(m_currentIndex - 1, 0);
        if (nextIndex.isValid() ) {
            if (isRowHidden(m_currentIndex - 1)) {
                nextIndex = model()->index(m_currentIndex - 2, 0);
            }
            setCurrentIndex(nextIndex);
            Q_EMIT clicked(nextIndex);
        }
        return;
    } else if (event->key() == Qt::Key_Down) {
        QModelIndex nextIndex = model()->index(m_currentIndex + 1, 0);
        if (nextIndex.isValid()) {
            if (isRowHidden(m_currentIndex + 1)) {
                nextIndex = model()->index(m_currentIndex + 2, 0);
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