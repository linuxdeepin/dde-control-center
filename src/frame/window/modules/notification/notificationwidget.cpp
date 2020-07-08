/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "notificationwidget.h"
#include "modules/notification/model/appitemmodel.h"
#include "modules/notification/notificationmodel.h"
#include "window/utils.h"

#include <DListView>

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QScroller>

DWIDGET_USE_NAMESPACE
using namespace dcc::notification;
using namespace DCC_NAMESPACE::notification;

NotificationWidget::NotificationWidget(NotificationModel *model, QWidget *parent)
    : QWidget(parent)
    , m_softwareListView(new DListView())
    , m_systemListView(new DListView())
    , m_sysmodel(new QStandardItemModel(this))
    , m_softwaremodel(new QStandardItemModel(this))
    , m_centralLayout(new QVBoxLayout())
    , m_model(model)
{
    setObjectName("notification");

    m_centralLayout->setMargin(0);

    m_systemListView->setMaximumHeight(50);
    m_systemListView->setResizeMode(QListView::Adjust);
    m_systemListView->setMovement(QListView::Static);
    m_systemListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    m_systemListView->setAutoScroll(false);
    m_systemListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_systemListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_systemListView->setViewportMargins(QMargins(0, 0, 13, 0));

    DStandardItem *systemitem = new DStandardItem(QIcon::fromTheme("dcc_general_purpose"), tr("System Notifications"));

    systemitem->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_sysmodel->appendRow(systemitem);
    m_systemListView->setModel(m_sysmodel);
    m_centralLayout->addWidget(m_systemListView);
    m_centralLayout->setContentsMargins(10,0,0,0);

    connect(m_systemListView, &DListView::clicked, this, &NotificationWidget::onSystemClicked);
    connect(m_systemListView, &DListView::activated, m_systemListView, &DListView::clicked);
    m_systemListView->setCurrentIndex(m_sysmodel->indexFromItem(m_sysmodel->item(0)));

    QLabel *themeL = new QLabel(tr("App Notifications"));
    themeL->setMargin(3);
    m_centralLayout->addWidget(themeL);
    m_softwareListView->setResizeMode(QListView::Adjust);
    m_softwareListView->setMovement(QListView::Static);
    m_softwareListView->setModel(m_softwaremodel);
    m_softwareListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    m_softwareListView->setViewportMargins(QMargins(0, 0, 13, 0));
    m_softwareListView->setSpacing(0);
    m_softwareListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QScroller::grabGesture(m_softwareListView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_softwareListView->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_theme = m_model->getTheme();

    //刷新数据
    refreshList();

    m_centralLayout->addWidget(m_softwareListView);
    setLayout(m_centralLayout);
    connect(m_softwareListView, &QListView::clicked, this, &NotificationWidget::onAppClicked);
    connect(m_softwareListView, &DListView::activated, m_softwareListView, &QListView::clicked);

    connect(m_model, &NotificationModel::appListChanged, this, &NotificationWidget::refreshList);
    connect(m_model, &NotificationModel::themeChanged, this, [ = ](const QString & theme) {
        m_theme = theme;
    });
}

void NotificationWidget::setModel(NotificationModel *model)
{
    m_model = model;
}

void NotificationWidget::onAppClicked(const QModelIndex &index)
{
    if (index.row() >= 0) {
        Q_EMIT requestShowApp(index.row());
        m_systemListView->clearSelection();
        m_softwareListView->setCurrentIndex(index);
    }
}

void NotificationWidget::setAppCurrentIndex(int row)
{
    if (row > m_softwaremodel->rowCount()) {
        row = 0;
    }
    m_softwareListView->setCurrentIndex(m_softwaremodel->indexFromItem(m_softwaremodel->item(row)));
}

void NotificationWidget::onSystemClicked(const QModelIndex &index)
{
    switch (index.row()) {
    case 0:
        Q_EMIT requestShowSystem();
        m_softwareListView->clearSelection();
        break;
    default:
        break;
    }
    m_systemListView->setCurrentIndex(index);
}

void NotificationWidget::refreshList()
{
    m_softwaremodel->clear();
    for (int i = 0; i < m_model->getAppSize(); ++i) {
        QString softName = m_model->getAppModel(i)->getAppName();
        QIcon icon = getAppIcon(m_model->getAppModel(i)->getIcon(), QSize(32, 32));
        DStandardItem *item = new DStandardItem(icon, softName);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_softwaremodel->appendRow(item);
    }
}

QIcon NotificationWidget::getAppIcon(const QString &appIcon, const QSize &size)
{
    QIcon icon = QIcon::fromTheme(appIcon, QIcon::fromTheme(m_theme, QIcon::fromTheme("application-x-desktop")));

    const qreal ratio = devicePixelRatioF();
    QPixmap pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}
