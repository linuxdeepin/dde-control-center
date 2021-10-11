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
    , m_loadingIndicator(new DSpinner)
    , m_parentView(listView)
    , m_editAction(new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true))
    , m_loadingAction(new DViewItemAction(Qt::AlignmentFlag::AlignRight, QSize(), QSize(), true))
    , m_itemData(nullptr)
    , m_connection(connection)
{
    Q_ASSERT(connection);

    m_loadingIndicator->setFixedSize(20, 20);
    m_loadingIndicator->setParent(m_parentView->viewport());

    QStyleOption opt;
    m_editAction->setIcon(DStyleHelper(widget->style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
    m_editAction->setClickAreaMargins(ArrowEnterClickMargin);

    m_loadingAction->setWidget(m_loadingIndicator);

    setActionList(Qt::Edge::RightEdge, { m_editAction, m_loadingAction });

    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    m_loadingAction->setVisible(false);
    connect(m_loadingAction, &DViewItemAction::destroyed, this, [ this ] {
       this->m_loadingAction = nullptr;
    });
    connect(m_loadingIndicator, &DSpinner::destroyed, this, [ this ] {
       this->m_loadingIndicator = nullptr;
    });

    connect(m_editAction, &QAction::triggered, [ = ] {
        Q_EMIT detailClick(m_connection);
    });
}

ConnectionPageItem::~ConnectionPageItem()
{
    if (m_loadingAction)
        m_loadingAction->setVisible(false);
    if (m_loadingIndicator)
        m_loadingIndicator->setVisible(false);
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
        m_loadingIndicator->start();
        m_loadingAction->setVisible(true);
    } else {
        m_editAction->setVisible(true);
        //m_loadingIndicator->stop();
        m_loadingIndicator->setVisible(false);
        m_loadingAction->setVisible(false);
    }

    m_parentView->update();
}
