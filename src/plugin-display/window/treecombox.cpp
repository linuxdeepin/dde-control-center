#include "cooperationsettingsdialog.h"
#include "treecombox.h"

#include <DStyle>
#include <QLabel>
#include <QPushButton>

#include <src/plugin-display/operation/displaymodel.h>
#include <src/plugin-display/operation/machine.h>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

TreeCombox::TreeCombox(QStandardItemModel *model, QWidget *parent)
    : DComboBox(parent)
    , m_deviceItemsListView(new DListView(this))
    , m_itemsmodel(model)
{
    initUI();
    initConnect();
}

TreeCombox::~TreeCombox()
{

}

void TreeCombox::initUI()
{
    this->setMaxVisibleItems(16);
    m_deviceItemsListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_deviceItemsListView->setFrameShape(QFrame::WinPanel);
    m_deviceItemsListView->setViewportMargins(10, 0, 10, 0);
    m_deviceItemsListView->setItemSpacing(0);
    m_deviceItemsListView->setMouseTracking(true);
    m_deviceItemsListView->setItemMargins(QMargins(10, 0, 10, 0));
    m_deviceItemsListView->setItemRadius(30);
    m_deviceItemsListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_deviceItemsListView->setItemRadius(0);
    m_deviceItemsListView->setBackgroundType(DStyledItemDelegate::BackgroundType::NoBackground);

    this->setModel(m_itemsmodel);
    this->setView(m_deviceItemsListView);
}

void TreeCombox::initConnect()
{
    connect(m_deviceItemsListView, &DListView::pressed, this, &TreeCombox::viewItemPressed);
}

void TreeCombox::addDivicesTitel(const QString &devTitel)
{
    QLabel *devLabel = new QLabel(devTitel, m_deviceItemsListView->viewport());
    QFont labelFt;
    labelFt.setPointSize(12);
    devLabel->setFont(labelFt);

    DStandardItem *item = new DStandardItem;
    DViewItemAction *action = new DViewItemAction();
    action->setWidget(devLabel);

    item->setActionList(Qt::Edge::LeftEdge,{action});
    item->setFlags(Qt::ItemIsUserCheckable);
    m_itemsmodel->appendRow(item);
}

void TreeCombox::addDevicesSettingsItem()
{
    QWidget *wid = new QWidget(m_deviceItemsListView->viewport());
    wid->setSizePolicy(QSizePolicy::Expanding,  QSizePolicy::Fixed);
    wid->setMinimumWidth(this->width());
    QVBoxLayout *layout = new QVBoxLayout(wid);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame *seperator = new QFrame(wid);

    seperator->setFrameStyle(QFrame::HLine | QFrame::Plain);
    seperator->setFrameShape(QFrame::HLine);
    seperator->setGeometry(m_deviceItemsListView->viewport()->rect());
    seperator->setMidLineWidth(m_deviceItemsListView->viewport()->width());
    seperator->setFixedHeight(2);

    QLabel *devLabel = new QLabel(tr("Collaboration Settings"), m_deviceItemsListView->viewport());
    QFont labelFt;
    labelFt.setPointSize(14);
    devLabel->setFont(labelFt);
    devLabel->setMinimumWidth(this->width());
    // TODO: 分隔符
    layout->addWidget(seperator);
    layout->addWidget(devLabel);

    wid->setLayout(layout);

    DStandardItem *item = new DStandardItem;
    DViewItemAction *action = new DViewItemAction;
    action->setWidget(wid);

    item->setActionList(Qt::Edge::LeftEdge, {action});
    item->setData(DevViewItemType::MoreSettingsItem, Qt::WhatsThisPropertyRole);
    item->setCheckable(true);

    m_itemsmodel->appendRow(item);
}

void TreeCombox::addDeviceCheckedIcon(Dtk::Widget::DStandardItem *pi,  bool initChecked)
{
    QSize size(14, 14);
    auto rightAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
    auto checkoutState = initChecked ? DStyle::SP_MarkElement : DStyle::SP_CustomBase;
    auto checkIcon = qobject_cast<DStyle *>(style())->standardIcon(checkoutState);
    rightAction->setIcon(checkIcon);
    pi->setActionList(Qt::Edge::LeftEdge, {rightAction});
}

void TreeCombox::updateItemCheckStatus(const QString &name, bool visible)
{
    for (int i = 0; i < m_itemsmodel->rowCount(); ++i) {
        auto item = static_cast<DStandardItem *>(m_itemsmodel->item(i));
        if (item->text() != name)
            continue;

        auto action = item->actionList(Qt::Edge::LeftEdge).first();
        qDebug() << "updateItemCheckStatus : " << name << visible;
        auto checkstatus = visible ? DStyle::SP_MarkElement : DStyle::SP_CustomBase;
        auto icon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
        action->setIcon(icon);
        m_deviceItemsListView->update(item->index());
        break;
    }
}
