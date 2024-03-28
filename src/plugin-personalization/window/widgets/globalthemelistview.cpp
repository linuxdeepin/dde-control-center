//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "globalthemelistview.h"
#include "model/thememodel.h"

#include <QApplication>
#include <QWheelEvent>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>

#include <DPaletteHelper>
#include <DStyle>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE
#define TAB_RADIUS 3

class GlobalThemeListViewPrivate
{
public:
    explicit GlobalThemeListViewPrivate(GlobalThemeListView *parent)
        : q_ptr(parent)
        , m_hSpacing(15)
        , m_vSpacing(15)
        , m_gridSize(160, 120)
        , m_itemSize(m_gridSize)
        , m_xOffset(0)
        , m_yOffset(0)
        , m_alignment(Qt::AlignHCenter | Qt::AlignTop)
        , m_currentPage(-1)
        , m_constPage(1)
        , m_drawStartPagePos(0, 285)
        , m_drawSpaacing(TAB_RADIUS * 4)
        , m_drawPageButton(LeftButton | RightButton)
    {
        m_rowPerPage = 2;
        m_colPerPage = 3;
        m_constPerPage = m_rowPerPage * m_colPerPage;
        setGridSize(m_gridSize);
        setPage(0);
        QObject::connect(q_ptr, &GlobalThemeListView::clicked, parent, [this](const QModelIndex &index) {
            if (index.isValid()) {
                Q_Q(GlobalThemeListView);
                emit q->applied(index);
            }
        });
    }

    void setSpacing(int space)
    {
        m_vSpacing = space;
    }
    int spacing() const
    {
        return m_vSpacing;
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
        Q_Q(GlobalThemeListView);
        updateTotal();

        m_itemSize = m_gridSize;
        m_hSpacing = (q->viewport()->width() - (m_colPerPage * m_itemSize.width())) / (m_colPerPage + 1);
        if (m_hSpacing < 0)
            m_hSpacing = 0;

        int itemWidth = m_colPerPage * (m_itemSize.width() + m_hSpacing) - m_hSpacing;

        if (m_alignment & Qt::AlignRight) {
            m_xOffset = q->viewport()->width() - itemWidth;
        } else if (m_alignment & Qt::AlignHCenter) {
            m_xOffset = (q->viewport()->width() - itemWidth) / 2;
        } else {
            m_xOffset = 0;
        }

        m_yOffset = m_vSpacing;

        int tabwidth = (m_constPage - 1) * m_drawSpaacing;
        m_drawStartPagePos.setX((q->width() - tabwidth) / 2);
        m_tabsRect = QRect(m_drawStartPagePos - QPoint(TAB_RADIUS, TAB_RADIUS), QSize(tabwidth + 2 * TAB_RADIUS, 2 * TAB_RADIUS));
        m_leftBtnRect = QRect(5, q->height() / 2 - 16, 32, 32);
        m_rightBtnRect = QRect(q->width() - 32 - 5, q->height() / 2 - 16, 32, 32);
    }
    // item在窗口中位置(无滚动)
    QRect rectForIndex(const QModelIndex &index) const
    {
        Q_Q(const GlobalThemeListView);
        QRect rect(0, 0, m_itemSize.width(), m_itemSize.height());
        int cnt = index.row();
        int page = cnt / m_constPerPage;
        int row = (cnt % m_constPerPage) / m_colPerPage;
        int col = (cnt % m_constPerPage) % m_colPerPage;
        rect.translate(q->width() * (page - m_currentPage) + m_xOffset + (m_itemSize.width() + m_hSpacing) * col, (m_itemSize.height() + m_vSpacing) * row);

        return rect.translated(q->contentsMargins().left(), q->contentsMargins().top() + m_yOffset);
    }
    // item在窗口中位置(无滚动)
    QModelIndex indexAt(const QPoint &p) const
    {
        Q_Q(const GlobalThemeListView);
        if ((m_itemSize.height() + m_vSpacing) <= 0 || (m_itemSize.width() + m_hSpacing) <= 0 || !q->model())
            return QModelIndex();
        QRect rect(p.x() - m_xOffset, p.y() - m_yOffset, 1, 1);
        int row = (rect.y()) / (m_itemSize.height() + m_vSpacing);
        int col = (rect.x()) / (m_itemSize.width() + m_hSpacing);

        int indexRow = m_currentPage * m_constPerPage + row * m_colPerPage + col;
        QModelIndex index = q->model()->index(indexRow, 0);
        if (index.isValid() && rectForIndex(index).contains(p)) {
            return index;
        }
        return QModelIndex();
    }
    QVector<QModelIndex> intersectingSet(const QRect &area) const
    {
        Q_Q(const GlobalThemeListView);
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
        Q_Q(const GlobalThemeListView);
        return q->contentsMargins().left() + q->contentsMargins().right();
    }
    inline int marginsHidget() const
    {
        Q_Q(const GlobalThemeListView);
        return q->contentsMargins().top() + q->contentsMargins().bottom();
    }
    bool updatePage(int add)
    {
        return setPage(m_currentPage + add);
    }
    bool setPage(int page)
    {
        int newPage = page;
        if (newPage < 0)
            newPage = 0;
        else if (newPage >= m_constPage)
            newPage = m_constPage - 1;
        if (newPage != m_currentPage) {
            m_currentPage = newPage;
            updateTotal();
            Q_Q(GlobalThemeListView);
            q->scheduleDelayedItemsLayout();
            updateHoverItem();
            return true;
        }
        return false;
    }
    void updateTotal()
    {
        Q_Q(GlobalThemeListView);
        int count = q->model() ? q->model()->rowCount() : 0;
        m_constPage = (count - 1) / m_constPerPage + 1;
        if (m_constPage < 1)
            m_constPage = 1;

        if (m_currentPage == 0)
            m_drawPageButton &= ~LeftButton;
        else
            m_drawPageButton |= LeftButton;

        if (m_currentPage == m_constPage - 1)
            m_drawPageButton &= ~RightButton;
        else
            m_drawPageButton |= RightButton;
    }
    void drawTabs(QPainter *painter, const QStyleOptionViewItem &option)
    {
        Q_UNUSED(option)
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(217, 217, 217)));
        for (int i = 0; i < m_constPage; i++) {
            painter->drawEllipse(QPoint(m_drawStartPagePos.x() + i * m_drawSpaacing, m_drawStartPagePos.y()), TAB_RADIUS, TAB_RADIUS);
        }
        painter->setBrush(QBrush(QColor(168, 168, 168)));
        painter->drawEllipse(QPoint(m_drawStartPagePos.x() + m_currentPage * m_drawSpaacing, m_drawStartPagePos.y()), TAB_RADIUS, TAB_RADIUS);
    }
    void drawPageButton(QPainter *painter, const QStyleOptionViewItem &option)
    {
        Q_Q(GlobalThemeListView);
        painter->setPen(Qt::NoPen);
        painter->setBrush(option.palette.midlight());
        if (m_drawPageButton & LeftButton) {
            if (m_drawPageButton & LeftBackground) {
                painter->drawEllipse(m_leftBtnRect);
            }
            DStyle::standardIcon(q->style(), DStyle::SP_ArrowLeave, &option, q).paint(painter, m_leftBtnRect.adjusted(8, 8, -8, -8));
        }
        if (m_drawPageButton & RightButton) {
            if (m_drawPageButton & RightBackground) {
                painter->drawEllipse(m_rightBtnRect);
            }
            DStyle::standardIcon(q->style(), DStyle::SP_ArrowEnter, &option, q).paint(painter, m_rightBtnRect.adjusted(8, 8, -8, -8));
        }
    }
    void updateHoverItem()
    {
        Q_Q(GlobalThemeListView);
        m_hover = indexAt(q->mapFromGlobal(QCursor::pos()));
    }
    enum PageButton : int {
        NoButton = 0x00000000,
        LeftButton = 0x00000001,
        LeftBackground = 0x00000002,
        RightButton = 0x00000004,
        RightBackground = 0x00000008,
        AllButton = LeftButton | RightButton,
    };

private:
    GlobalThemeListView *const q_ptr;
    Q_DECLARE_PUBLIC(GlobalThemeListView)
    int m_hSpacing;
    int m_vSpacing;
    QSize m_gridSize;

    QSize m_itemSize;
    int m_xOffset;             // x轴偏移
    int m_yOffset;             // y轴偏移
    QModelIndex m_hover;       // hover项
    Qt::Alignment m_alignment; //　对齐方式

    int m_constPerPage; // 每页个数 =m_rowPerPage*m_colPerPage
    int m_rowPerPage;   // 每页行数
    int m_colPerPage;   // 每页列数
    int m_currentPage;  // 当前页
    int m_constPage;    // 总页数

    QPoint m_drawStartPagePos; // 第一页位置
    int m_drawSpaacing;        // 页签间隔
    int m_drawPageButton;      // 右右翻页按钮

    QRect m_leftBtnRect;
    QRect m_rightBtnRect;
    QRect m_tabsRect;
};

GlobalThemeListView::GlobalThemeListView(QWidget *parent)
    : QAbstractItemView(parent)
    , DCC_INIT_PRIVATE(GlobalThemeListView)
{
    setFrameShape(QFrame::NoFrame);
    viewport()->setAutoFillBackground(false);
    setAutoFillBackground(false);
    setBackgroundRole(viewport()->backgroundRole());
    setMouseTracking(true);

    GlobalThemeDelegate *delegate = new GlobalThemeDelegate(this);
    delegate->setBackgroundType(DStyledItemDelegate::RoundedBackground);
    delegate->setItemSpacing(10);
    setItemDelegate(delegate);

    setIconSize(QSize(155, 88));
    setGridSize(QSize(160, 120));
    setFixedHeight(300);
    setMinimumWidth(500);
}

GlobalThemeListView::~GlobalThemeListView()
{
}

void GlobalThemeListView::setThemeModel(ThemeModel *model)
{
    GlobalThemeModel *viewModel = new GlobalThemeModel(this);
    viewModel->setThemeModel(model);
    setModel(viewModel);
}

void GlobalThemeListView::setSpacing(int space)
{
    Q_D(GlobalThemeListView);
    if (d->spacing() != space) {
        d->setSpacing(space);
        scheduleDelayedItemsLayout();
    }
}

int GlobalThemeListView::spacing() const
{
    Q_D(const GlobalThemeListView);
    return d->spacing();
}

void GlobalThemeListView::setGridSize(const QSize &size)
{
    Q_D(GlobalThemeListView);
    if (d->gridSize() != size) {
        d->setGridSize(size);
        scheduleDelayedItemsLayout();
    }
}

QSize GlobalThemeListView::gridSize() const
{
    Q_D(const GlobalThemeListView);
    return d->gridSize();
}

QRect GlobalThemeListView::visualRect(const QModelIndex &index) const
{
    Q_D(const GlobalThemeListView);
    return d->rectForIndex(index).translated(-horizontalOffset(), -verticalOffset());
}

void GlobalThemeListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(hint)
    if (!index.isValid())
        return;
    Q_D(GlobalThemeListView);
    int page = index.row() / d->m_constPerPage;

    d->setPage(page);
}

QModelIndex GlobalThemeListView::indexAt(const QPoint &p) const
{
    Q_D(const GlobalThemeListView);
    return d->indexAt(p + QPoint(horizontalOffset(), verticalOffset()));
}

QModelIndex GlobalThemeListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(const GlobalThemeListView);
    Q_UNUSED(modifiers)
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
        int pageItem = (viewport()->height() - d->marginsHidget() + d->m_vSpacing) / (d->m_itemSize.height() + d->m_vSpacing);
        for (int i = 0; i < pageItem; i++) {
            currentRow = moveup(currentRow, d->m_colPerPage);
        }
    } break;
    case MoveUp:
        currentRow = moveup(currentRow, d->m_colPerPage);
        break;
    case MovePageDown: {
        int pageItem = (viewport()->height() - d->marginsHidget() + d->m_vSpacing) / (d->m_itemSize.height() + d->m_vSpacing);
        for (int i = 0; i < pageItem; i++) {
            int row = movedown(currentRow, d->m_colPerPage);
            if (row >= maxRow)
                break;
            currentRow = row;
        }
    } break;
    case MoveDown:
        currentRow = movedown(currentRow, d->m_colPerPage);
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

int GlobalThemeListView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int GlobalThemeListView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool GlobalThemeListView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return false;
}

QRegion GlobalThemeListView::visualRegionForSelection(const QItemSelection &selection) const
{
    if (selection.isEmpty())
        return QRegion();
    Q_D(const GlobalThemeListView);
    QRect rect = d->rectForIndex(selection.indexes().first());
    return QRegion(rect);
}

void GlobalThemeListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
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

void GlobalThemeListView::updateGeometries()
{
    Q_D(GlobalThemeListView);
    QAbstractItemView::updateGeometries();
    d->updateGeometries();

    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);
}

void GlobalThemeListView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
    scheduleDelayedItemsLayout();
}

void GlobalThemeListView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    scheduleDelayedItemsLayout();
    QAbstractItemView::rowsInserted(parent, start, end);
}

void GlobalThemeListView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
    scheduleDelayedItemsLayout();
}

void GlobalThemeListView::paintEvent(QPaintEvent *e)
{
    Q_D(GlobalThemeListView);
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
    if (!d->m_tabsRect.intersected(e->rect()).isEmpty())
        d->drawTabs(&painter, option);
    if (!d->m_leftBtnRect.intersected(e->rect()).isEmpty()
        || !d->m_rightBtnRect.intersected(e->rect()).isEmpty())
        d->drawPageButton(&painter, option);
}

bool GlobalThemeListView::viewportEvent(QEvent *event)
{
    Q_D(GlobalThemeListView);
    switch (event->type()) {
    case QEvent::HoverLeave:
    case QEvent::Leave:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
        d->updateHoverItem();
        break;
    default:
        break;
    }
    return QAbstractItemView::viewportEvent(event);
}

void GlobalThemeListView::wheelEvent(QWheelEvent *e)
{
    Q_D(GlobalThemeListView);
    if (d->updatePage((e->angleDelta().y() < 0) ? 1 : -1))
        e->setAccepted(true);
}

void GlobalThemeListView::mousePressEvent(QMouseEvent *event)
{
    Q_D(GlobalThemeListView);
    const QPoint &pos = event->pos();
    if (d->m_leftBtnRect.contains(pos)) {
        d->updatePage(-1);
        event->setAccepted(true);
        return;
    } else if (d->m_rightBtnRect.contains(pos)) {
        d->updatePage(1);
        event->setAccepted(true);
        return;
    } else if (d->m_tabsRect.contains(pos)) {
        for (int i = 0; i < d->m_constPage; i++) {
            if (std::abs((d->m_drawStartPagePos.x() + i * d->m_drawSpaacing) - pos.x()) < TAB_RADIUS) {
                d->setPage(i);
                event->setAccepted(true);
                return;
            }
        }
    }

    QAbstractItemView::mousePressEvent(event);
}

void GlobalThemeListView::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(GlobalThemeListView);
    if (d->m_leftBtnRect.contains(event->pos())) {
        if (!(d->m_drawPageButton & GlobalThemeListViewPrivate::LeftBackground)) {
            d->m_drawPageButton |= GlobalThemeListViewPrivate::LeftBackground;
            update(d->m_leftBtnRect);
        }
    } else {
        if (d->m_drawPageButton & GlobalThemeListViewPrivate::LeftBackground) {
            d->m_drawPageButton &= ~GlobalThemeListViewPrivate::LeftBackground;
            update(d->m_leftBtnRect);
        }
    }

    if (d->m_rightBtnRect.contains(event->pos())) {
        if (!(d->m_drawPageButton & GlobalThemeListViewPrivate::RightBackground)) {
            d->m_drawPageButton |= GlobalThemeListViewPrivate::RightBackground;
            update(d->m_rightBtnRect);
        }
    } else {
        if (d->m_drawPageButton & GlobalThemeListViewPrivate::RightBackground) {
            d->m_drawPageButton &= ~GlobalThemeListViewPrivate::RightBackground;
            update(d->m_rightBtnRect);
        }
    }
    QAbstractItemView::mouseMoveEvent(event);
}
/////////////////////////////////////////
GlobalThemeModel::GlobalThemeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_themeModel(nullptr)
{
}

void GlobalThemeModel::setThemeModel(ThemeModel *model)
{
    m_themeModel = model;
    connect(m_themeModel, &ThemeModel::defaultChanged, this, &GlobalThemeModel::updateData);
    connect(m_themeModel, &ThemeModel::picAdded, this, &GlobalThemeModel::updateData);
    connect(m_themeModel, &ThemeModel::itemAdded, this, &GlobalThemeModel::updateData);
    connect(m_themeModel, &ThemeModel::itemRemoved, this, &GlobalThemeModel::updateData);
    updateData();
}

QModelIndex GlobalThemeModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_keys.size())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex GlobalThemeModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int GlobalThemeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_keys.size();

    return 0;
}

int GlobalThemeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant GlobalThemeModel::data(const QModelIndex &index, int role) const
{
    if (m_keys.isEmpty() || !index.isValid())
        return QVariant();

    int row = index.row();
    switch (role) {
    case Qt::DisplayRole:
        return m_themeModel->getList().value(m_keys.at(row))["Name"].toString();
    case Qt::ToolTipRole:
        return m_themeModel->getList().value(m_keys.at(row))["Comment"].toString();
    case Qt::DecorationRole:
        return QIcon(m_themeModel->getPicList().value(m_keys.at(row)));
    case Qt::CheckStateRole: {
        QString id = m_themeModel->getDefault();
        if (id.endsWith(".light")) {
            id.chop(6);
        } else if (id.endsWith(".dark")) {
            id.chop(5);
        }
        return m_keys.at(row) == id ? Qt::Checked : Qt::Unchecked;
    }
    case GlobalThemeModel::IdRole:
        return m_keys.at(row);
    default:
        break;
    }
    return QVariant();
}

void GlobalThemeModel::updateData()
{
    QStringList keys = m_themeModel->keys();
    if (keys.contains("custom")) {
        keys.removeAll("custom");
        keys.push_back("custom");
    }
    beginResetModel();
    m_keys = keys;
    endResetModel();
}
/////////////////////////////
GlobalThemeDelegate::GlobalThemeDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
}

void GlobalThemeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    // 选择高亮背景
    if (opt.state & QStyle::State_Selected) {
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                ? QPalette::Normal
                : QPalette::Disabled;
        opt.backgroundBrush = option.palette.color(cg, QPalette::Highlight);
    }
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    QRect decorationRect;
    decorationRect = QRect(opt.rect.topLeft() + QPoint((opt.rect.width() - opt.decorationSize.width()) / 2, 6), opt.decorationSize);
    opt.displayAlignment = Qt::AlignCenter;

    QRect displayRect = QRect(opt.rect.topLeft() + QPoint(0, opt.decorationSize.height() + 15), QSize(opt.rect.width(), 15));

    // draw the item
    if (index.data(Qt::CheckStateRole) == Qt::Checked)
        drawChecked(style, painter, opt, decorationRect);
    // 图标的绘制用也可能会使用这些颜色
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
    painter->setPen(opt.palette.color(cg, QPalette::Text));
    drawDecoration(painter, opt, decorationRect);

    drawDisplay(style, painter, opt, displayRect);
    painter->restore();
}

void GlobalThemeDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if (option.features & QStyleOptionViewItem::HasDecoration) {
        QIcon::Mode mode = QIcon::Normal;
        if (!(option.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if (option.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        QIcon::State state = (option.state & QStyle::State_Open) ? QIcon::On : QIcon::Off;
        painter->save();
        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 8, 8);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setClipPath(painterPath);
        option.icon.paint(painter, rect, option.decorationAlignment, mode, state);

        painter->restore();
    }
}

void GlobalThemeDelegate::drawChecked(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(style)
    QRect r = rect;
    r.adjust(-4, -4, 4, 4);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen(option.palette.color(QPalette::Normal, QPalette::Highlight), 3);
    painter->setPen(pen);
    painter->drawRoundedRect(r, 8, 8);
    painter->restore();
}

void GlobalThemeDelegate::drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    DStyle::viewItemDrawText(style, painter, &option, rect);
}
