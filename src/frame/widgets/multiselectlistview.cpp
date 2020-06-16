#include "widgets/multiselectlistview.h"

namespace dcc {
namespace widgets {

MultiSelectListView::MultiSelectListView(QWidget *parent): DListView(parent) {

}

void MultiSelectListView::resetStatus(const QModelIndex &index) {
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

}
}
