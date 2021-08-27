#include "connectionpageitem.h"
#include "window/utils.h"

#include <DListView>
#include <DSpinner>
#include <DStyleHelper>

#include <networkconst.h>

DWIDGET_USE_NAMESPACE
using namespace dde::network;

ConnectionPageItem::ConnectionPageItem(QWidget *widget, DListView *listView, Connection *connection)
    : DStandardItem()
    , m_parentView(listView)
    , m_itemData(nullptr)
    , m_connection(connection)
{
    Q_ASSERT(connection);

    m_loadingIndicator = new DSpinner;
    m_loadingIndicator->setFixedSize(20, 20);
    m_loadingIndicator->setParent(m_parentView->viewport());
    m_loadingIndicator->hide();

    m_editAction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
    QStyleOption opt;
    m_editAction->setIcon(DStyleHelper(widget->style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
    m_editAction->setClickAreaMargins(ArrowEnterClickMargin);

    setActionList(Qt::Edge::RightEdge, { m_editAction });

    connect(m_editAction, &QAction::triggered, [ = ] {
        Q_EMIT detailClick(m_connection);
    });

    connect(m_parentView, &QListView::clicked, [ = ](const QModelIndex &idx) {
        if (idx == this->index())
            Q_EMIT this->itemClick(m_connection);
    });
}

ConnectionPageItem::~ConnectionPageItem()
{
}

void ConnectionPageItem::setItemData(void *data)
{
    m_itemData = data;
}

void *ConnectionPageItem::itemData()
{
    return m_itemData;
}

void ConnectionPageItem::setConnectionStatus(const ConnectionStatus &status)
{
    // 设置当前的连接状态
    switch (status) {
    case ConnectionStatus::Activated:
        setLoading(false);
        setCheckState(Qt::CheckState::Checked);
        break;
    case ConnectionStatus::Activating:
        setLoading(true);
        setCheckState(Qt::CheckState::Unchecked);
        break;
    default:
        setLoading(false);
        setCheckState(Qt::CheckState::Unchecked);
        break;
    }
}

void ConnectionPageItem::setLoading(const bool isLoading)
{
    if (isLoading) {
        m_editAction->setVisible(false);
        QRect itemrect = m_parentView->visualRect(index());
        int leftPos = itemrect.x() + itemrect.width() - m_loadingIndicator->width() - 10;
        int topPos = itemrect.y() + (itemrect.height() - m_loadingIndicator->height()) / 2;
        QPoint point(leftPos, topPos);
        m_loadingIndicator->move(point);
        m_loadingIndicator->start();
        m_loadingIndicator->show();
    } else {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_editAction->setVisible(true);
    }

    m_parentView->update();
}
