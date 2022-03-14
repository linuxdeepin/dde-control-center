/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "listview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QModelIndex>

#include <DApplicationHelper>
#include <DPalette>
#include <DObjectPrivate>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

/////////////////////////////////////////
namespace DCC_NAMESPACE {

class ListViewPrivate : public DObjectPrivate
{
public:
    ListViewPrivate(ListView *listview)
        : DObjectPrivate(listview)
        , m_spacing(20)
        , m_gridSize(280, 84)
        , m_viewMode(ListView::ListMode)
        , m_itemSize(m_gridSize)
        , m_maxColumnCount(1)
        , m_maxRowCount(1)
        , m_xOffset(0)
        , m_yOffset(0)
        , m_alignment(Qt::AlignLeft | Qt::AlignTop)
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
        m_firstHeightDiff = m_viewMode == ListView::IconMode ? 0 : 18;
    }
    QSize gridSize() const
    {
        return m_gridSize;
    }

    void setViewMode(ListView::ViewMode mode)
    {
        m_viewMode = mode;
    }
    ListView::ViewMode viewMode() const
    {
        return m_viewMode;
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
        D_Q(ListView);
        if (m_viewMode == ListView::IconMode) {
            m_maxColumnCount = (q->viewport()->width() - m_spacing) / (m_gridSize.width() + m_spacing);
            if (m_maxColumnCount <= 0)
                m_maxColumnCount = 1;
        } else {
            m_maxColumnCount = 1;
        }

        int count = q->model() ? q->model()->rowCount() : 0;
        if (count < m_maxColumnCount) {
            m_maxColumnCount = count;
        }

        if (m_viewMode == ListView::IconMode) {
            if (count == 0)
                m_maxRowCount = 0;
            else if (count <= m_maxColumnCount)
                m_maxRowCount = 2;
            else
                m_maxRowCount = 1 + (count / m_maxColumnCount);
        } else {
            m_maxRowCount = (count <= m_maxColumnCount) ? 1 : count / m_maxColumnCount;
        }

        m_itemSize = (m_viewMode == ListView::IconMode) ? m_gridSize : QSize(q->viewport()->width() - marginsWidth(), m_gridSize.height());
        int itemWidth = m_maxColumnCount * (m_itemSize.width() + m_spacing) - m_spacing;
        int itemHeight = m_maxRowCount * (m_itemSize.height() + m_spacing) - m_spacing;

        if (m_alignment & Qt::AlignRight) {
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
        D_Q(const ListView);
        QRect rect(0, 0, m_itemSize.width(), m_itemSize.height());
        if (index.row() == 0 && m_viewMode == ListView::IconMode) {
            rect.setHeight(m_itemSize.height() * 2 + m_spacing);
        } else if (index.row() == 0 && m_viewMode == ListView::ListMode) {
            rect.setHeight(m_itemSize.height() + m_firstHeightDiff);
        } else {
            int indexRow = index.row();
            if (m_viewMode == ListView::IconMode && indexRow >= m_maxColumnCount)
                indexRow++;
            int row = indexRow / m_maxColumnCount;
            int col = indexRow % m_maxColumnCount;
            rect.translate((m_itemSize.width() + m_spacing) * col, (m_itemSize.height() + m_spacing) * row);
            if (m_viewMode == ListView::ListMode && indexRow >= 1)
                rect.translate(0, m_firstHeightDiff);
        }
        return rect.translated(q->contentsMargins().left() + m_xOffset, q->contentsMargins().top() + m_yOffset);
    }
    // item在窗口中位置(无滚动)
    QModelIndex indexAt(const QPoint &p) const
    {
        D_Q(const ListView);
        QRect rect(p.x() - m_xOffset, p.y() - m_yOffset, 1, 1);
        int row = (rect.y() - m_firstHeightDiff) / (m_itemSize.height() + m_spacing);
        int col = (rect.x()) / (m_itemSize.width() + m_spacing);
        if (row < 0)
            row = 0;
        int indexRow = 0;
        if (m_viewMode == ListView::IconMode) {
            if (row != 1 || col != 0) {
                indexRow = row > 0 ? row * m_maxColumnCount + col - 1 : col;
            }
        } else {
            indexRow = row * m_maxColumnCount + col;
        }
        QModelIndex index = q->model()->index(indexRow, 0);
        if (index.isValid() && rectForIndex(index).contains(p))
            return index;
        return QModelIndex();
    }
    QVector<QModelIndex> intersectingSet(const QRect &area) const
    {
        D_Q(const ListView);
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
        D_Q(const ListView);
        return q->contentsMargins().left() + q->contentsMargins().right();
    }
    inline int marginsHidget() const
    {
        D_Q(const ListView);
        return q->contentsMargins().top() + q->contentsMargins().bottom();
    }

    D_DECLARE_PUBLIC(ListView)
public:
    int m_spacing;
    QSize m_gridSize;
    ListView::ViewMode m_viewMode;

    QSize m_itemSize;
    int m_maxColumnCount;      // 一行可容纳的最大列数
    int m_maxRowCount;         // 换算显示所有item所需行数
    int m_xOffset;             // x轴偏移
    int m_yOffset;             // y轴偏移
    QModelIndex m_hover;       // hover项
    Qt::Alignment m_alignment; //　对齐方式
    int m_firstHeightDiff;     // 第一行与其他行高差值
};
} // namespace DCC_NAMESPACE

/////////////////////////////////////////

ListView::ListView(QWidget *parent)
    : QAbstractItemView(parent)
    , DObject(*new ListViewPrivate(this), this)
{
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    scheduleDelayedItemsLayout();
    setMouseTracking(true);
}

ListView::~ListView()
{
}

void ListView::setSpacing(int space)
{
    D_D(ListView);
    if (d->spacing() != space) {
        d->setSpacing(space);
        scheduleDelayedItemsLayout();
    }
}
int ListView::spacing() const
{
    D_D(const ListView);
    return d->spacing();
}

void ListView::setGridSize(const QSize &size)
{
    D_D(ListView);
    if (d->gridSize() != size) {
        d->setGridSize(size);
        scheduleDelayedItemsLayout();
    }
}
QSize ListView::gridSize() const
{
    D_D(const ListView);
    return d->gridSize();
}

void ListView::setViewMode(ViewMode mode)
{
    D_D(ListView);
    if (d->viewMode() != mode) {
        d->setViewMode(mode);
        scheduleDelayedItemsLayout();
    }
}
ListView::ViewMode ListView::viewMode() const
{
    D_D(const ListView);
    return d->viewMode();
}

void ListView::setAlignment(Qt::Alignment alignment)
{
    D_D(ListView);
    if (d->alignment() != alignment) {
        d->setAlignment(alignment);
        scheduleDelayedItemsLayout();
    }
}
Qt::Alignment ListView::alignment() const
{
    D_D(const ListView);
    return d->alignment();
}
/////////////////////////////////////////////////////////////////////////////
// item在窗口中位置(加滚动偏移)
QRect ListView::visualRect(const QModelIndex &index) const
{
    D_D(const ListView);
    return d->rectForIndex(index).translated(-horizontalOffset(), -verticalOffset());
}

void ListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    if (!index.isValid())
        return;

    const QRect rect = visualRect(index);
    if (hint == EnsureVisible && viewport()->rect().contains(rect)) {
        viewport()->update(rect);
        return;
    }

    const QRect area = viewport()->rect();
    const bool above = (hint == EnsureVisible && rect.top() < area.top());
    const bool below = (hint == EnsureVisible && rect.bottom() > area.bottom());

    int verticalValue = verticalScrollBar()->value();
    QRect adjusted = rect.adjusted(-spacing(), -spacing(), spacing(), spacing());
    if (hint == PositionAtTop || above)
        verticalValue += adjusted.top();
    else if (hint == PositionAtBottom || below)
        verticalValue += qMin(adjusted.top(), adjusted.bottom() - area.height() + 1);
    else if (hint == PositionAtCenter)
        verticalValue += adjusted.top() - ((area.height() - adjusted.height()) / 2);
    verticalScrollBar()->setValue(verticalValue);
}

QModelIndex ListView::indexAt(const QPoint &p) const
{
    D_D(const ListView);
    return d->indexAt(p + QPoint(horizontalOffset(), verticalOffset()));
}

QModelIndex ListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/)
{
    D_D(const ListView);
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
    case MovePrevious:
        currentRow--;
        break;
    case MoveRight:
    case MoveNext:
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
    }
    return model()->index(currentRow, 0);
}

int ListView::horizontalOffset() const
{
    return 0;
}

int ListView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

void ListView::updateGeometries()
{
    D_D(ListView);
    QAbstractItemView::updateGeometries();
    d->updateGeometries();

    //　更新滚动条范围
    if (geometry().isEmpty() || !model() || model()->rowCount() <= 0 || model()->columnCount() <= 0) {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QSize step = d->m_itemSize;
        verticalScrollBar()->setSingleStep(step.height() + spacing());
        verticalScrollBar()->setPageStep(viewport()->height());

        int height = d->m_maxRowCount * (d->m_itemSize.height() + d->m_spacing) - d->m_spacing;
        if (height < viewport()->height()) {
            verticalScrollBar()->setRange(0, 0);
        } else {
            verticalScrollBar()->setRange(0, height - viewport()->height());
        }
    }
}

bool ListView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

QRegion ListView::visualRegionForSelection(const QItemSelection &selection) const
{
    if (selection.isEmpty())
        return QRegion();
    D_D(const ListView);
    QRect rect = d->rectForIndex(selection.indexes().first());
    return QRegion(rect);
}

void ListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
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

void ListView::paintEvent(QPaintEvent *e)
{
    D_D(const ListView);
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
    option.decorationAlignment = d->m_viewMode == IconMode ? Qt::AlignCenter : Qt::AlignLeft;

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
}

void ListView::mouseMoveEvent(QMouseEvent *e)
{
    if (!isVisible())
        return;
    QAbstractItemView::mouseMoveEvent(e);
    if (state() == ExpandingState || state() == CollapsingState)
        return;
    D_D(ListView);
    d->m_hover = indexAt(e->pos());
}

void ListView::leaveEvent(QEvent *)
{
    D_D(ListView);
    d->m_hover = QModelIndex();
}
