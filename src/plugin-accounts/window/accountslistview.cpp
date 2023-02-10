//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "accountslistview.h"

#include <QWheelEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QApplication>

#include <QDebug>

class AccountsListViewPrivate
{
public:
    explicit AccountsListViewPrivate(AccountsListView *parent)
        : q_ptr(parent)
        , m_spacing(20)
        , m_gridSize(64, 64)
        , m_itemSize(m_gridSize)
        , m_maxColumnCount(1)
        , m_maxRowCount(1)
        , m_xOffset(0)
        , m_yOffset(0)
        , m_alignment(Qt::AlignHCenter)
        , m_firstHeightDiff(0)
    {
        setGridSize(m_gridSize);
    }

    void setSpacing(int space)
    {
        m_spacing = space;
    }
    int spacing() const
    {
        return m_spacing;
    }

    void setGridSize(const QSize &size)
    {
        m_gridSize = size;
        m_firstHeightDiff = 0;
    }
    QSize gridSize() const
    {
        return m_gridSize;
    }

    void setAlignment(Qt::Alignment alignment)
    {
        m_alignment = alignment;
    }
    Qt::Alignment alignment() const
    {
        return m_alignment;
    }

    void updateGeometries()
    {
        Q_Q(AccountsListView);
        m_maxColumnCount = 1;

        int count = q->model() ? q->model()->rowCount() : 0;
        m_maxRowCount = (count <= m_maxColumnCount) ? 1 : count;

        m_itemSize = m_gridSize;
        int itemWidth = m_maxRowCount * (m_itemSize.width() + m_spacing) - m_spacing;
        int itemHeight = m_maxColumnCount * (m_itemSize.height() + m_spacing) - m_spacing;

        if (itemWidth > q->viewport()->width()) {
            m_xOffset = 0;
        } else if (m_alignment & Qt::AlignRight) {
            m_xOffset = q->viewport()->width() - itemWidth;
        } else if (m_alignment & Qt::AlignHCenter) {
            m_xOffset = (q->viewport()->width() - itemWidth) / 2;
        } else {
            m_xOffset = 0;
        }
        if (itemHeight > q->viewport()->height()) {
            m_yOffset = 0;
        } else if (m_alignment & Qt::AlignBottom) {
            m_yOffset = q->viewport()->height() - itemHeight;
        } else if (m_alignment & Qt::AlignVCenter) {
            m_yOffset = (q->viewport()->height() - itemHeight) / 2;
        } else {
            m_yOffset = 0;
        }
    }
    // item在窗口中位置(无滚动)
    QRect rectForIndex(const QModelIndex &index) const
    {
        Q_Q(const AccountsListView);
        QRect rect(0, 0, m_itemSize.width(), m_itemSize.height());
        //        if (index.row() == 0 && m_viewMode == AccountsListView::IconMode) {
        //            rect.setHeight(m_itemSize.height() * 2 + m_spacing);
        //        } else if (index.row() == 0 && m_viewMode == AccountsListView::ListMode) {
        //            rect.setHeight(m_itemSize.height() + m_firstHeightDiff);
        //        } else {
        int indexRow = index.row();
        //            if (m_viewMode == AccountsListView::IconMode && indexRow >= m_maxColumnCount)
        //                indexRow++;
        //            int row = indexRow / m_maxColumnCount;
        //            int col = indexRow % m_maxColumnCount;
        rect.translate((m_itemSize.width() + m_spacing) * indexRow, (m_spacing));
        //            if (m_viewMode == AccountsListView::ListMode && indexRow >= 1)
        //                rect.translate(0, m_firstHeightDiff);
        //        }
        return rect.translated(q->contentsMargins().left() + m_xOffset, q->contentsMargins().top() + m_yOffset);
    }
    // item在窗口中位置(无滚动)
    QModelIndex indexAt(const QPoint &p) const
    {
        if ((m_itemSize.height() + m_spacing) <= 0 || (m_itemSize.width() + m_spacing) <= 0)
            return QModelIndex();
        Q_Q(const AccountsListView);
        QRect rect(p.x() - m_xOffset, p.y() - m_yOffset, 1, 1);
        //        int row = (rect.y() - m_firstHeightDiff) / (m_itemSize.height() + m_spacing);
        int col = (rect.x()) / (m_itemSize.width() + m_spacing);

        QModelIndex index = q->model()->index(col, 0);
        if (index.isValid() && rectForIndex(index).contains(p))
            return index;
        return QModelIndex();
    }
    QVector<QModelIndex> intersectingSet(const QRect &area) const
    {
        Q_Q(const AccountsListView);
        QVector<QModelIndex> indexs;
        int rows = q->model()->rowCount();
        for (int row = 0; row < rows; row++) {
            QModelIndex index = q->model()->index(row, 0);
            QRect rectIndex = rectForIndex(index);
            if (!rectIndex.intersected(area).isEmpty()) {
                indexs.append(index);
            }
        }
        return indexs;
    }
    inline int marginsWidth() const
    {
        Q_Q(const AccountsListView);
        return q->contentsMargins().left() + q->contentsMargins().right();
    }
    inline int marginsHidget() const
    {
        Q_Q(const AccountsListView);
        return q->contentsMargins().top() + q->contentsMargins().bottom();
    }

private:
    AccountsListView *const q_ptr;
    Q_DECLARE_PUBLIC(AccountsListView)
    int m_spacing;
    QSize m_gridSize;

    QSize m_itemSize;
    int m_maxColumnCount;      // 一行可容纳的最大列数
    int m_maxRowCount;         // 换算显示所有item所需行数
    int m_xOffset;             // x轴偏移
    int m_yOffset;             // y轴偏移
    QModelIndex m_hover;       // hover项
    Qt::Alignment m_alignment; //　对齐方式
    int m_firstHeightDiff;     // 第一行与其他行高差值
};

/////////////////////////////////////////

AccountsListView::AccountsListView(QWidget *parent)
    : QAbstractItemView(parent)
    , DCC_INIT_PRIVATE(AccountsListView)
{
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    scheduleDelayedItemsLayout();
    setMouseTracking(true);
}

AccountsListView::~AccountsListView()
{
}

void AccountsListView::setModel(QAbstractItemModel *model)
{
    setVisible(model->rowCount() >= 2);
    QAbstractItemView::setModel(model);
}

/////////////////////////////////////////////////////////////////////////////
// item在窗口中位置(加滚动偏移)
QRect AccountsListView::visualRect(const QModelIndex &index) const
{
    Q_D(const AccountsListView);
    return d->rectForIndex(index).translated(-horizontalOffset(), -verticalOffset());
}

void AccountsListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    if (!index.isValid())
        return;

    const QRect rect = visualRect(index);
    if (hint == EnsureVisible && viewport()->rect().contains(rect)) {
        viewport()->update(rect);
        return;
    }

    const QRect area = viewport()->rect();
    const bool above = (hint == EnsureVisible && rect.left() < area.left());
    const bool below = (hint == EnsureVisible && rect.right() > area.right());

    Q_D(const AccountsListView);
    int horizontalValue = horizontalScrollBar()->value();
    QRect adjusted = rect.adjusted(-d->spacing(), -d->spacing(), d->spacing(), d->spacing());
    if (hint == PositionAtTop || above)
        horizontalValue += adjusted.left();
    else if (hint == PositionAtBottom || below)
        horizontalValue += qMin(adjusted.left(), adjusted.right() - area.width() + 1);
    else if (hint == PositionAtCenter)
        horizontalValue += adjusted.left() - ((area.width() - adjusted.width()) / 2);
    horizontalScrollBar()->setValue(horizontalValue);
}

QModelIndex AccountsListView::indexAt(const QPoint &p) const
{
    Q_D(const AccountsListView);
    return d->indexAt(p + QPoint(horizontalOffset(), verticalOffset()));
}

QModelIndex AccountsListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/)
{
    Q_D(const AccountsListView);
    QModelIndex current = currentIndex();
    int currentRow = current.row();
    int maxRow = model()->rowCount();

    auto moveup = [](int currentRow, int maxColumnCount) -> int {
        if (currentRow == maxColumnCount * 2 - 1) {
            currentRow = 0;
        } else if (currentRow < maxColumnCount) {
            // 第一行不处理
        } else if (currentRow < maxColumnCount * 2) {
            currentRow -= (maxColumnCount - 1);
        } else {
            currentRow -= maxColumnCount;
        }
        return currentRow;
    };
    auto movedown = [](int currentRow, int maxColumnCount) -> int {
        if (currentRow == 0) {
            currentRow += (maxColumnCount * 2 - 1);
        } else if (currentRow < maxColumnCount)
            currentRow += (maxColumnCount - 1);
        else
            currentRow += maxColumnCount;
        return currentRow;
    };
    switch (cursorAction) {
    case MoveLeft:
        currentRow--;
        break;
    case MoveRight:
        currentRow++;
        break;
    case MovePageUp: {
        int pageItem = (viewport()->height() - d->marginsHidget() + d->m_spacing) / (d->m_itemSize.height() + d->m_spacing);
        for (int i = 0; i < pageItem; i++) {
            currentRow = moveup(currentRow, d->m_maxColumnCount);
        }
    } break;
    case MoveUp:
        currentRow = moveup(currentRow, d->m_maxColumnCount);
        break;
    case MovePageDown: {
        int pageItem = (viewport()->height() - d->marginsHidget() + d->m_spacing) / (d->m_itemSize.height() + d->m_spacing);
        for (int i = 0; i < pageItem; i++) {
            int row = movedown(currentRow, d->m_maxColumnCount);
            if (row >= maxRow)
                break;
            currentRow = row;
        }
    } break;
    case MoveDown:
        currentRow = movedown(currentRow, d->m_maxColumnCount);
        break;
    case MoveHome:
        currentRow = 0;
        break;
    case MoveEnd:
        currentRow = maxRow - 1;
        break;
    default:
        return QModelIndex();
    }
    QModelIndex selectIndex = model()->index(currentRow, 0);
    activated(selectIndex);
    return selectIndex;
}

int AccountsListView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int AccountsListView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

void AccountsListView::updateGeometries()
{
    Q_D(AccountsListView);
    QAbstractItemView::updateGeometries();
    d->updateGeometries();

    //　更新滚动条范围
    if (geometry().isEmpty() || !model() || model()->rowCount() <= 0 || model()->columnCount() <= 0) {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QSize step = d->m_itemSize;

        horizontalScrollBar()->setSingleStep(step.width() + d->spacing());
        horizontalScrollBar()->setPageStep(viewport()->width());

        int width = d->m_maxRowCount * (d->m_itemSize.width() + d->m_spacing) - d->m_spacing;
        if (width < viewport()->width()) {
            horizontalScrollBar()->setRange(0, 0);
        } else {
            horizontalScrollBar()->setRange(0, width - viewport()->width());
        }
    }
}

void AccountsListView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
    scheduleDelayedItemsLayout();
}

void AccountsListView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    scheduleDelayedItemsLayout();
    QAbstractItemView::rowsInserted(parent, start, end);
    if (model()->rowCount() >= 2)
        setVisible(true);
}

void AccountsListView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
    scheduleDelayedItemsLayout();
    if (model()->rowCount() <= 2)
        setVisible(false);
}

bool AccountsListView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

QRegion AccountsListView::visualRegionForSelection(const QItemSelection &selection) const
{
    if (selection.isEmpty())
        return QRegion();
    Q_D(const AccountsListView);
    QRect rect = d->rectForIndex(selection.indexes().first());
    return QRegion(rect);
}

void AccountsListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    int rows = model()->rowCount();
    QModelIndex selectedIndex;
    for (int row = 0; row < rows; row++) {
        QModelIndex index = model()->index(row, 0);
        QRect rectIndex = visualRect(index);
        if (!rectIndex.intersected(rect).isEmpty()) {
            selectedIndex = index;
            break;
        }
    }
    selectionModel()->select(selectedIndex, command);
}

void AccountsListView::paintEvent(QPaintEvent *e)
{
    Q_D(const AccountsListView);
    QStyleOptionViewItem option = viewOptions();
    QPainter painter(viewport());

    const QVector<QModelIndex> toBeRendered = d->intersectingSet(e->rect().translated(horizontalOffset(), verticalOffset()));

    const QModelIndex current = currentIndex();
    const QModelIndex hover = d->m_hover;
    const QAbstractItemModel *itemModel = model();
    const QItemSelectionModel *selections = selectionModel();
    const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
    const bool alternate = alternatingRowColors();
    const QStyle::State state = option.state;
    const QAbstractItemView::State viewState = this->state();
    const bool enabled = (state & QStyle::State_Enabled) != 0;
    option.decorationAlignment = Qt::AlignCenter;

    bool alternateBase = false;
    int previousRow = -2;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(e->rect(), palette().color(QPalette::Window));

    QVector<QModelIndex>::const_iterator end = toBeRendered.constEnd();
    for (QVector<QModelIndex>::const_iterator it = toBeRendered.constBegin(); it != end; ++it) {
        Q_ASSERT((*it).isValid());
        option.rect = visualRect(*it);

        option.state = state;
        if (selections && selections->isSelected(*it))
            option.state |= QStyle::State_Selected;
        if (enabled) {
            QPalette::ColorGroup cg;
            if ((itemModel->flags(*it) & Qt::ItemIsEnabled) == 0) {
                option.state &= ~QStyle::State_Enabled;
                cg = QPalette::Disabled;
            } else {
                cg = QPalette::Normal;
            }
            option.palette.setCurrentColorGroup(cg);
        }
        if (focus && current == *it) {
            option.state |= QStyle::State_HasFocus;
            if (viewState == EditingState)
                option.state |= QStyle::State_Editing;
        }
        option.state.setFlag(QStyle::State_MouseOver, *it == hover);

        if (alternate) { //　交替色处理，未实现
            int row = (*it).row();
            if (row != previousRow + 1) {
                // adjust alternateBase according to rows in the "gap"
                alternateBase = (row & 1) != 0;
            }
            option.features.setFlag(QStyleOptionViewItem::Alternate, alternateBase);

            // draw background of the item (only alternate row). rest of the background
            // is provided by the delegate
            QStyle::State oldState = option.state;
            option.state &= ~QStyle::State_Selected;
            style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, &painter, this);
            option.state = oldState;

            alternateBase = !alternateBase;
            previousRow = row;
        }

        itemDelegate(*it)->paint(&painter, option, *it);
    }
    QScrollBar *hbar = horizontalScrollBar();
    int gradualW = d->m_itemSize.width() * 2;
    QRect lRect(0, 0, gradualW, height());
    QRect rRect(width() - gradualW, 0, gradualW + 1, height());
    if (!e->rect().intersected(lRect).isEmpty() && hbar->minimum() != hbar->value()) { //左边
        QLinearGradient linearGradient(lRect.left(), 0, lRect.right(), 0);
        linearGradient.setColorAt(0, option.palette.window().color());
        linearGradient.setColorAt(1, QColor(255, 255, 255, 0));
        painter.setBrush(QBrush(linearGradient));
        painter.setPen(Qt::NoPen);
        painter.drawRect(lRect);
    }
    if (!e->rect().intersected(rRect).isEmpty() && hbar->maximum() != hbar->value()) { // 右边
        QLinearGradient linearGradient(rRect.left(), 0, rRect.right(), 0);
        linearGradient.setColorAt(0, QColor(255, 255, 255, 0));
        linearGradient.setColorAt(1, option.palette.window().color());
        painter.setBrush(QBrush(linearGradient));
        painter.setPen(Qt::NoPen);
        painter.drawRect(rRect);
    }
}

bool AccountsListView::viewportEvent(QEvent *event)
{
    Q_D(AccountsListView);
    switch (event->type()) {
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
        d->m_hover = indexAt(static_cast<QHoverEvent *>(event)->pos());
        break;
    case QEvent::HoverLeave:
    case QEvent::Leave:
        d->m_hover = QModelIndex();
        break;
    default:
        break;
    }
    return QAbstractItemView::viewportEvent(event);
}

void AccountsListView::wheelEvent(QWheelEvent *e)
{
    QApplication::sendEvent(horizontalScrollBar(), e);
    e->setAccepted(true);
}
