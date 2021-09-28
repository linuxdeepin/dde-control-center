/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "loginoptionswidget.h"
#include "widgets/multiselectlistview.h"
#include "window/gsettingwatcher.h"
#include "window/utils.h"

#include <QBoxLayout>
#include <QScroller>
#include <QStandardItemModel>


using namespace dcc::widgets;
using namespace DCC_NAMESPACE::authentication;

LoginOptionsWidget::LoginOptionsWidget(QWidget *parent)
    : QWidget (parent)
    , m_deviceListView(new dcc::widgets::MultiSelectListView(this))
    , m_deviceItemModel(new QStandardItemModel(this))
{
    setObjectName("Suthentication");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    initUI();
    initMembers();
    initConnections();
}

LoginOptionsWidget::~LoginOptionsWidget()
{
    if (m_deviceItemModel) {
        m_deviceItemModel->clear();
        m_deviceItemModel->deleteLater();
        m_deviceItemModel = nullptr;
    }
}

void LoginOptionsWidget::showDefaultWidget()
{
    for (int i = 0; i < m_deviceListView->model()->rowCount(); i++) {
        if (!m_deviceListView->isRowHidden(i)) {
            m_deviceListView->activated(m_deviceListView->model()->index(i, 0));
            break;
        }
    }
}

void LoginOptionsWidget::onItemClicked(const QModelIndex &index)
{
    if (index == m_currentIndex)
        return;

    m_currentIndex = index;
    m_menuMethod[index.row()].itemSignal.invoke(m_menuMethod[index.row()].plugin ? m_menuMethod[index.row()].plugin : this);
    m_deviceListView->resetStatus(index);
}

void LoginOptionsWidget::initUI()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout();
    mainContentLayout->setMargin(0);
    mainContentLayout->addWidget(m_deviceListView);

    m_deviceListView->setAccessibleName("List_userlist");
    m_deviceListView->setFrameShape(QFrame::NoFrame);
    m_deviceListView->setViewportMargins(ScrollAreaMargins);
    m_deviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_deviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_deviceListView->setEditTriggers(QListView::NoEditTriggers);
    m_deviceListView->setDragEnabled(false);
    m_deviceListView->setIconSize(QSize(30, 30));
    m_deviceListView->setLayoutDirection(Qt::LeftToRight);
    m_deviceListView->setModel(m_deviceItemModel);

    QScroller::grabGesture(m_deviceListView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_deviceListView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    setLayout(mainContentLayout);
}

void LoginOptionsWidget::initMembers()
{
    //~ contents_path /authentication/Finger
    m_menuMethod.append({"dcc_fingerprint", tr("Finger"), QMetaMethod::fromSignal(&LoginOptionsWidget::requestShowFingerDetail), nullptr, "authenticationFinger"});

    for (auto mm : m_menuMethod) {
        DStandardItem *item = new DStandardItem(mm.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setIcon(QIcon::fromTheme(mm.itemIcon));
        m_deviceItemModel->appendRow(item);
        GSettingWatcher::instance()->bind(mm.gsettingsName, m_deviceListView, item);
    }
}

void LoginOptionsWidget::initConnections()
{
    connect(m_deviceListView, &QListView::clicked, this, &LoginOptionsWidget::onItemClicked);
    connect(m_deviceListView, &DListView::activated, m_deviceListView, &QListView::clicked);
}
