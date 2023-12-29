//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "cooperationsettingsdialog.h"
#include "treecombox.h"

#include <DStyle>
#include <QLabel>
#include <QPushButton>

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
    TreeComboxDelegate *delegate = new TreeComboxDelegate(m_deviceItemsListView);
    delegate->setMargins(QMargins(0, 0, 0, 0));
    m_deviceItemsListView->setItemDelegate(delegate);

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
    DStandardItem *item = new DStandardItem;
    DViewItemAction *action = new DViewItemAction;
    action->setWidget(devLabel);

    item->setActionList(Qt::Edge::LeftEdge,{action});
    item->setFontSize(DFontSizeManager::SizeType::T6);
    item->setFlags(Qt::ItemIsUserCheckable);
    m_itemsmodel->appendRow(item);
}

void TreeCombox::addDevicesSettingsItem()
{
    DStandardItem *pi = new DStandardItem;
    pi->setText(tr("Collaboration Settings"));
    pi->setTextColorRole(DPalette::ColorType::TextTitle);
    pi->setFontSize(DFontSizeManager::SizeType::T5);

    QSize size(14, 14);
    auto rightAction = new DViewItemAction(Qt::AlignVCenter, size, size, true);
    pi->setActionList(Qt::Edge::LeftEdge, {rightAction});
    m_itemsmodel->appendRow(pi);
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

TreeComboxDelegate::TreeComboxDelegate(QAbstractItemView *parent)
    : Dtk::Widget::DStyledItemDelegate(parent)
    , m_parentWidget(parent)
{
    m_parentWidget->update();
}

TreeComboxDelegate::~TreeComboxDelegate()
{

}

void TreeComboxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row() == m_parentWidget->model()->rowCount() - 2) {
        // 绘制间隔线
        QRect rct = option.rect;
        rct.setY(rct.top() + rct.height() - 1);
        rct.setHeight(2);
        painter->fillRect(rct, QColor(0, 0, 0, static_cast<int>(0.1 * 255)));
    }
    DStyledItemDelegate::paint(painter, option, index);
}
