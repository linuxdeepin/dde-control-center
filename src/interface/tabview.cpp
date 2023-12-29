//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "tabview.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QModelIndex>
#include <QApplication>

#include <DPalette>
#include <DStyleHelper>
#include <DStyle>
#include <DStyleOption>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

/////////////////////////////////////////
namespace DCC_NAMESPACE {

class TabViewPrivate
{
public:
    explicit TabViewPrivate(TabView *parent)
        : q_ptr(parent)
        , m_spacing(20)
        , m_gridSize(280, 84)
        , m_maxColumnCount(1)
        , m_maxRowCount(1)
        , m_xOffset(0)
        , m_yOffset(0)
        , m_alignment(Qt::AlignHCenter)
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
        Q_Q(TabView);
        m_itemX.clear();
        int totalWidth = 0;
        int height = 0;
        QFontMetrics fm = q->fontMetrics();
        QAbstractItemModel *model = q->model();
        DStyleHelper dstyle(q->style());
        DStyleOptionButtonBoxButton opt;

        QSize emptySZ = fm.size(Qt::TextShowMnemonic, QStringLiteral("XXXX"));
        emptySZ = (dstyle.sizeFromContents(DStyle::CT_ButtonBoxButton, &opt, emptySZ, q).expandedTo(QApplication::globalStrut()));
        for (int i = 0; i < model->rowCount(); i++) {
            QString s(model->data(model->index(i, 0)).toString());
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
        height = emptySZ.height() - 9;
        m_size = QSize(totalWidth, height);
        q->setFixedHeight(m_size.height() + 4);

        if (totalWidth > q->viewport()->width()) {
            m_xOffset = 0;
        } else if (m_alignment & Qt::AlignRight) {
            m_xOffset = q->viewport()->width() - totalWidth;
        } else if (m_alignment & Qt::AlignHCenter) {
            m_xOffset = (q->viewport()->width() - totalWidth) / 2;
        } else {
            m_xOffset = 0;
        }
        if (height > q->viewport()->height()) {
            m_yOffset = 0;
        } else if (m_alignment & Qt::AlignBottom) {
            m_yOffset = q->viewport()->height() - height;
        } else if (m_alignment & Qt::AlignVCenter) {
            m_yOffset = (q->viewport()->height() - height) / 2;
        } else {
            m_yOffset = 0;
        }
    }
    // item在窗口中位置(无滚动)
    QRect rectForIndex(const QModelIndex &index) const
    {
        Q_Q(const TabView);
        QRect rect(0, 2, 0, m_size.height());
        int indexRow = index.row();
        if (indexRow < 0 || indexRow >= m_itemX.size()) {
            rect = QRect();
        } else if (indexRow == 0) {
            rect.setWidth(m_itemX.at(0));
        } else {
            rect.setLeft(m_itemX.at(indexRow - 1));
            rect.setRight(m_itemX.at(indexRow) - 1);
        }

        return rect.translated(q->contentsMargins().left() + m_xOffset, q->contentsMargins().top() + m_yOffset);
    }
    // item在窗口中位置(无滚动)
    QModelIndex indexAt(const QPoint &p) const
    {
        Q_Q(const TabView);
        QRect rect(p.x() - m_xOffset, p.y() - m_yOffset, 1, 1);
        if (!QRect(QPoint(), m_size).contains(rect)) {
            return QModelIndex();
        }
        int row = -1;
        for (int i = 0; i < m_itemX.size(); i++) {
            if (rect.x() <= m_itemX.at(i)) {
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
        Q_Q(const TabView);
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
        Q_Q(const TabView);
        return q->contentsMargins().left() + q->contentsMargins().right();
    }
    inline int marginsHidget() const
    {
        Q_Q(const TabView);
        return q->contentsMargins().top() + q->contentsMargins().bottom();
    }

private:
    TabView *const q_ptr;
    Q_DECLARE_PUBLIC(TabView)
    int m_spacing;
    QSize m_gridSize;

    int m_maxColumnCount;      // 一行可容纳的最大列数
    int m_maxRowCount;         // 换算显示所有item所需行数
    int m_xOffset;             // x轴偏移
    int m_yOffset;             // y轴偏移
    QModelIndex m_hover;       // hover项
    Qt::Alignment m_alignment; //　对齐方式

    QList<int> m_itemX;
    QSize m_size;
};
} // namespace DCC_NAMESPACE

/////////////////////////////////////////

TabView::TabView(QWidget *parent)
    : QAbstractItemView(parent)
    , d_ptr(new TabViewPrivate(this))
{
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    scheduleDelayedItemsLayout();
    setMouseTracking(true);
    setContentsMargins(0, 0, 0, 0);
    setFrameStyle(QFrame::NoFrame);

    // setFixedHeight at the beginning
    DStyleHelper dstyle(style());
    QFontMetrics fm = fontMetrics();
    QSize emptySZ = fm.size(Qt::TextShowMnemonic, QStringLiteral("XXXX"));
    DStyleOptionButtonBoxButton opt;
    emptySZ = (dstyle.sizeFromContents(DStyle::CT_ButtonBoxButton, &opt, emptySZ, this).expandedTo(QApplication::globalStrut()));
    setFixedHeight(emptySZ.height() - 5);

    viewport()->setAutoFillBackground(false);
}

TabView::~TabView()
{
    delete d_ptr;
}

void TabView::setSpacing(int space)
{
    Q_D(TabView);
    if (d->spacing() != space) {
        d->setSpacing(space);
        scheduleDelayedItemsLayout();
    }
}
int TabView::spacing() const
{
    Q_D(const TabView);
    return d->spacing();
}

void TabView::setGridSize(const QSize &size)
{
    Q_D(TabView);
    if (d->gridSize() != size) {
        d->setGridSize(size);
        scheduleDelayedItemsLayout();
    }
}
QSize TabView::gridSize() const
{
    Q_D(const TabView);
    return d->gridSize();
}

void TabView::setAlignment(Qt::Alignment alignment)
{
    Q_D(TabView);
    if (d->alignment() != alignment) {
        d->setAlignment(alignment);
        scheduleDelayedItemsLayout();
    }
}
Qt::Alignment TabView::alignment() const
{
    Q_D(const TabView);
    return d->alignment();
}
/////////////////////////////////////////////////////////////////////////////
// item在窗口中位置(加滚动偏移)
QRect TabView::visualRect(const QModelIndex &index) const
{
    Q_D(const TabView);
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
    Q_D(const TabView);
    return d->indexAt(p + QPoint(horizontalOffset(), verticalOffset()));
}

QModelIndex TabView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/)
{
    Q_D(const TabView);
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
    default:
        return QModelIndex();
    }
    QModelIndex selectIndex = model()->index(currentRow, 0);
    Q_EMIT activated(selectIndex);
    return selectIndex;
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
    Q_D(TabView);
    QAbstractItemView::updateGeometries();
    d->updateGeometries();

    //　更新滚动条范围
    if (geometry().isEmpty() || !model() || model()->rowCount() <= 0 || model()->columnCount() <= 0) {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QSize step = QSize(d->m_itemX.isEmpty() ? 0 : d->m_itemX.first(), d->m_size.height());
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

void TabView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
    scheduleDelayedItemsLayout();
}

void TabView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    scheduleDelayedItemsLayout();
    QAbstractItemView::rowsInserted(parent, start, end);
}

void TabView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
    scheduleDelayedItemsLayout();
}

bool TabView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

QRegion TabView::visualRegionForSelection(const QItemSelection &selection) const
{
    if (selection.isEmpty())
        return QRegion();
    Q_D(const TabView);
    QRect rect = d->rectForIndex(selection.indexes().first());
    return QRegion(rect);
}

void TabView::setSelection([[maybe_unused]] const QRect &rect, [[maybe_unused]] QItemSelectionModel::SelectionFlags command)
{

}

void TabView::paintEvent(QPaintEvent *e)
{
    Q_D(const TabView);
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
    option.decorationAlignment = Qt::AlignLeft;

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

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(palette().color(QPalette::Base)));
        QRect maskRect = option.rect.adjusted(0, -2, 0, 2);
        if (it->row() == 0) {
            painter.drawRoundedRect(option.rect.adjusted(-2, -2, 0, 2), 8, 8);
            maskRect.adjust(6, 0, 0, 0);
        }
        if (it->row() == itemModel->rowCount() - 1) {
            painter.drawRoundedRect(option.rect.adjusted(0, -2, 2, 2), 8, 8);
            maskRect.adjust(0, 0, -6, 0);
        }
        painter.drawRect(maskRect);
        painter.restore();

        itemDelegate(*it)->paint(&painter, option, *it);
    }
}

bool TabView::viewportEvent(QEvent *event)
{
    Q_D(TabView);
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

void TabView::wheelEvent(QWheelEvent *e)
{
    QApplication::sendEvent(horizontalScrollBar(), e);
    e->setAccepted(true);
}

void TabView::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}
