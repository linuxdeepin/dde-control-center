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
#include "tabview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QModelIndex>
#include <QApplication>

#include <DApplicationHelper>
#include <DPalette>
#include <DObjectPrivate>
#include <DStyleHelper>
#include <DStyle>
#include <DStyleOption>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

/////////////////////////////////////////
namespace DCC_NAMESPACE {

class TabViewPrivate : public DObjectPrivate
{
public:
    explicit TabViewPrivate(TabView *TabView)
        : DObjectPrivate(TabView)
        , m_spacing(20)
        , m_gridSize(280, 84)
        , m_viewMode(TabView::ListMode)
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
        m_firstHeightDiff = m_viewMode == TabView::IconMode ? 0 : 18;
    }
    QSize gridSize() const
    {
        return m_gridSize;
    }

    void setViewMode(TabView::ViewMode mode)
    {
        m_viewMode = mode;
    }
    TabView::ViewMode viewMode() const
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
        D_Q(TabView);
        m_itemX.clear();
        int totalWidth = 0;
        int height = 0;
        QFontMetrics fm = q->fontMetrics();
        QAbstractItemModel *model = q->model();
        DStyleHelper dstyle(q->style());
        DStyleOptionButtonBoxButton opt;

        QSize emptySZ = fm.size(Qt::TextShowMnemonic, QStringLiteral("XXXX"));
        emptySZ = (dstyle.sizeFromContents(DStyle::CT_ButtonBoxButton, &opt, emptySZ, q).expandedTo(QApplication::globalStrut()));
        for (int i =0;i<model->rowCount();i++) {
            QString s(model->data(model->index(i,0)).toString());
            QSize sz;
            if (s.isEmpty()) {
                sz = emptySZ;
            } else {
                sz = fm.size(Qt::TextShowMnemonic, s);
                sz = (dstyle.sizeFromContents(DStyle::CT_ButtonBoxButton, &opt, sz, q).expandedTo(QApplication::globalStrut()));
            }
            totalWidth += sz.width() + 14;
            m_itemX.append(totalWidth);
        }
        height = emptySZ.height()-9;
        m_size = QSize(totalWidth,height);
        q->setFixedSize(m_size.width()+2,m_size.height()+2);
    }
    // item在窗口中位置(无滚动)
    QRect rectForIndex(const QModelIndex &index) const
    {
        D_Q(const TabView);
        QRect rect(0, 0, 0, m_size.height());
        int indexRow = index.row();
        if (indexRow < 0 || indexRow >= m_itemX.size()) {
            rect = QRect();
        } else if (indexRow == 0) {
            rect.setWidth(m_itemX.at(0));
        } else {
            rect.setLeft(m_itemX.at(indexRow - 1));
            rect.setRight(m_itemX.at(indexRow));
        }

        return rect.translated(q->contentsMargins().left() + m_xOffset, q->contentsMargins().top() + m_yOffset);
    }
    // item在窗口中位置(无滚动)
    QModelIndex indexAt(const QPoint &p) const
    {
        D_Q(const TabView);
        if(!QRect(QPoint(),m_size).contains(p) ) {
            return QModelIndex();
        }
        int row = -1;
        for (int i = 0; i < m_itemX.size(); i++) {
            if (p.x() <= m_itemX.at(i)) {
                row = i;
                break;
            }
        }
        QModelIndex index = q->model() ? q->model()->index(row, 0) : QModelIndex();
        if (index.isValid() && rectForIndex(index).contains(p))
            return index;
        return QModelIndex();
    }
    QVector<QModelIndex> intersectingSet(const QRect &area) const
    {
        D_Q(const TabView);
        QVector<QModelIndex> indexs;
        int rows = q->model() ? q->model()->rowCount() : 0;
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
        D_Q(const TabView);
        return q->contentsMargins().left() + q->contentsMargins().right();
    }
    inline int marginsHidget() const
    {
        D_Q(const TabView);
        return q->contentsMargins().top() + q->contentsMargins().bottom();
    }

    D_DECLARE_PUBLIC(TabView)
public:
    int m_spacing;
    QSize m_gridSize;
    TabView::ViewMode m_viewMode;

//    QSize m_itemSize;
    int m_maxColumnCount;      // 一行可容纳的最大列数
    int m_maxRowCount;         // 换算显示所有item所需行数
    int m_xOffset;             // x轴偏移
    int m_yOffset;             // y轴偏移
    QModelIndex m_hover;       // hover项
    Qt::Alignment m_alignment; //　对齐方式
    int m_firstHeightDiff;     // 第一行与其他行高差值

    QList<int> m_itemX;
    QSize m_size;
};
} // namespace DCC_NAMESPACE

/////////////////////////////////////////

TabView::TabView(QWidget *parent)
    : QAbstractItemView(parent)
    , DObject(*new TabViewPrivate(this), this)
{
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    scheduleDelayedItemsLayout();
    setMouseTracking(true);
    setContentsMargins(0,0,0,0);
    setFrameStyle(QFrame::NoFrame);
}

TabView::~TabView()
{
}

void TabView::setSpacing(int space)
{
    D_D(TabView);
    if (d->spacing() != space) {
        d->setSpacing(space);
        scheduleDelayedItemsLayout();
    }
}
int TabView::spacing() const
{
    D_D(const TabView);
    return d->spacing();
}

void TabView::setGridSize(const QSize &size)
{
    D_D(TabView);
    if (d->gridSize() != size) {
        d->setGridSize(size);
        scheduleDelayedItemsLayout();
    }
}
QSize TabView::gridSize() const
{
    D_D(const TabView);
    return d->gridSize();
}

void TabView::setViewMode(ViewMode mode)
{
    D_D(TabView);
    if (d->viewMode() != mode) {
        d->setViewMode(mode);
        scheduleDelayedItemsLayout();
    }
}
TabView::ViewMode TabView::viewMode() const
{
    D_D(const TabView);
    return d->viewMode();
}

void TabView::setAlignment(Qt::Alignment alignment)
{
    D_D(TabView);
    if (d->alignment() != alignment) {
        d->setAlignment(alignment);
        scheduleDelayedItemsLayout();
    }
}
Qt::Alignment TabView::alignment() const
{
    D_D(const TabView);
    return d->alignment();
}
/////////////////////////////////////////////////////////////////////////////
// item在窗口中位置(加滚动偏移)
QRect TabView::visualRect(const QModelIndex &index) const
{
    D_D(const TabView);
    return d->rectForIndex(index).translated(-horizontalOffset(), -verticalOffset());
}

void TabView::scrollTo(const QModelIndex &index, ScrollHint hint)
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

    int horizontalValue = horizontalScrollBar()->value();
    QRect adjusted = rect.adjusted(-spacing(), -spacing(), spacing(), spacing());
    if (hint == PositionAtTop || above)
        horizontalValue += adjusted.top();
    else if (hint == PositionAtBottom || below)
        horizontalValue += qMin(adjusted.left(), adjusted.right() - area.width() + 1);
    else if (hint == PositionAtCenter)
        horizontalValue += adjusted.left() - ((area.width() - adjusted.width()) / 2);
    horizontalScrollBar()->setValue(horizontalValue);
}

QModelIndex TabView::indexAt(const QPoint &p) const
{
    D_D(const TabView);
    return d->indexAt(p + QPoint(horizontalOffset(), verticalOffset()));
}

QModelIndex TabView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/)
{
    D_D(const TabView);
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
    } break;
    case MoveUp:
        currentRow = moveup(currentRow, d->m_maxColumnCount);
        break;
    case MovePageDown: {
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

int TabView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int TabView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

void TabView::updateGeometries()
{
    D_D(TabView);
    QAbstractItemView::updateGeometries();
    d->updateGeometries();

    //　更新滚动条范围
    if (geometry().isEmpty() || !model() || model()->rowCount() <= 0 || model()->columnCount() <= 0) {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QSize step = QSize(d->m_itemX.isEmpty()?0:d->m_itemX.first(),d->m_size.height());
        horizontalScrollBar()->setSingleStep(step.width() + spacing());
        horizontalScrollBar()->setPageStep(viewport()->width());

        int width = d->m_size.width();
        if (width < viewport()->width()) {
            horizontalScrollBar()->setRange(0, 0);
        } else {
            horizontalScrollBar()->setRange(0, width - viewport()->width());
        }
    }
}

bool TabView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

QRegion TabView::visualRegionForSelection(const QItemSelection &selection) const
{
    if (selection.isEmpty())
        return QRegion();
    D_D(const TabView);
    QRect rect = d->rectForIndex(selection.indexes().first());
    return QRegion(rect);
}

void TabView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
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

void TabView::paintEvent(QPaintEvent *e)
{
    D_D(const TabView);
    QStyleOptionViewItem option = viewOptions();
    QPainter painter(viewport());

    const QVector<QModelIndex> toBeRendered = d->intersectingSet(e->rect().translated(horizontalOffset(), verticalOffset()));

    const QModelIndex current = currentIndex();
    const QModelIndex hover = d->m_hover;
    const QAbstractItemModel *itemModel = model();
    const QItemSelectionModel *selections = selectionModel();
    const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
    const QStyle::State state = option.state;
    const QAbstractItemView::State viewState = this->state();
    const bool enabled = (state & QStyle::State_Enabled) != 0;
    option.decorationAlignment = d->m_viewMode == IconMode ? Qt::AlignCenter : Qt::AlignLeft;

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

        itemDelegate(*it)->paint(&painter, option, *it);
    }
}

void TabView::mouseMoveEvent(QMouseEvent *e)
{
    if (!isVisible())
        return;
    QAbstractItemView::mouseMoveEvent(e);
    if (state() == ExpandingState || state() == CollapsingState)
        return;
    D_D(TabView);
    d->m_hover = indexAt(e->pos());
}

void TabView::leaveEvent(QEvent *)
{
    D_D(TabView);
    d->m_hover = QModelIndex();
}
