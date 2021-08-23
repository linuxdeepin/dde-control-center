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

#include <QSvgRenderer>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QScroller>
#include <QFile>
#include <QGSettings>

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
    , m_setting(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
{
    setObjectName("notification");

    this->setAccessibleName("NotificationWidget");
    m_softwareListView->setAccessibleName("softwareListView");
    m_systemListView->setAccessibleName("systemListView");

    m_centralLayout->setMargin(0);

    m_systemListView->setMaximumHeight(50);
    m_systemListView->setResizeMode(QListView::Adjust);
    m_systemListView->setMovement(QListView::Static);
    m_systemListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    m_systemListView->setAutoScroll(false);
    m_systemListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_systemListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_systemListView->setViewportMargins(QMargins(0, 0, 8, 0));
    m_systemListView->setIconSize(ListViweIconSize);

    //~ contents_path /notification/System Notifications
    //~ child_page System Notifications
    DStandardItem *systemitem = new DStandardItem(QIcon::fromTheme("dcc_general_purpose"), tr("System Notifications"));

    systemitem->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_sysmodel->appendRow(systemitem);
    m_systemListView->setModel(m_sysmodel);
    m_centralLayout->addWidget(m_systemListView, 0, Qt::AlignTop);
    m_centralLayout->addSpacing(0);
    m_centralLayout->setContentsMargins(10,10,0,0);

    connect(m_systemListView, &DListView::clicked, this, &NotificationWidget::onSystemClicked);
    connect(m_systemListView, &DListView::activated, m_systemListView, &DListView::clicked);
    m_lastIndex = m_sysmodel->indexFromItem(m_sysmodel->item(0));
    m_systemListView->setCurrentIndex(m_lastIndex);

    m_appTitleLable = new QLabel(tr("App Notifications"));
    m_appTitleLable->setMargin(3);
    m_centralLayout->addWidget(m_appTitleLable);
    m_softwareListView->setResizeMode(QListView::Adjust);
    m_softwareListView->setMovement(QListView::Static);
    m_softwareListView->setModel(m_softwaremodel);
    m_softwareListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    m_softwareListView->setViewportMargins(QMargins(0, 0, 9, 0));
    m_softwareListView->setSpacing(0);
    m_softwareListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_softwareListView->setIconSize(ListViweIconSize);

    QScroller::grabGesture(m_softwareListView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_softwareListView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    //刷新数据
    refreshList();

    m_centralLayout->addWidget(m_softwareListView);
    setLayout(m_centralLayout);
    connect(m_softwareListView, &QListView::clicked, this, &NotificationWidget::onAppClicked);
    connect(m_softwareListView, &DListView::activated, m_softwareListView, &QListView::clicked);

    connect(m_model, &NotificationModel::appListChanged, this, &NotificationWidget::refreshList);
    connect(m_setting, &QGSettings::changed, this,  &NotificationWidget::onSettingChanged);
}

void NotificationWidget::onAppClicked(const QModelIndex &index)
{
    if (index.row() >= 0) {
        m_systemListView->clearSelection();
        if (m_lastIndex == index) return;

        m_lastIndex = index;
        Q_EMIT requestShowApp(index.row());
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
    if (m_lastIndex == index) return;

    m_lastIndex = index;
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

void NotificationWidget::showDefaultWidget()
{
    bool isShowSystemNotify = m_setting->get("systemNotification").toBool();
    bool isShowAppNotify = m_setting->get("appNotifications").toBool();
    m_systemListView->setVisible(isShowSystemNotify);
    m_softwareListView->setVisible(isShowAppNotify);
    m_appTitleLable->setVisible(isShowAppNotify);
    if (isShowSystemNotify) {
        onSystemClicked(m_sysmodel->index(0, 0));
        Q_EMIT requestShowSystem();
    } else if (isShowAppNotify) {
        onAppClicked(m_softwaremodel->index(0, 0));
    }
}

void NotificationWidget::onSettingChanged(const QString &key)
{
    bool isShow;
    if (key == "systemNotification") {
        isShow = m_setting->get("systemNotification").toBool();

        if (isShow == m_systemListView->isVisible())
            return;
        m_systemListView->setVisible(isShow);

        if (!isShow) {
            if (!m_softwareListView->isVisible()) {
                Q_EMIT popWidget();
            } else if (m_lastIndex.model() == m_softwaremodel) {
                onAppClicked(m_lastIndex);
            } else if (m_softwareListView->isVisible()) {
                onAppClicked(m_softwaremodel->index(0, 0));
            }
        } else if (!m_softwareListView->isVisible()) {
            onSystemClicked(m_sysmodel->index(0, 0));
            Q_EMIT requestShowSystem();
        }
    } else if (key == "appNotifications") {
        isShow = m_setting->get("appNotifications").toBool();

        if (isShow == m_softwareListView->isVisible())
            return;
        m_softwareListView->setVisible(isShow);
        m_appTitleLable->setVisible(isShow);

        if (!isShow) {
            if (!m_systemListView->isVisible()) {
                Q_EMIT popWidget();
            } else if (m_lastIndex.model() != m_sysmodel) {
                onSystemClicked(m_sysmodel->index(0, 0));
            }
        } else if (!m_systemListView->isVisible()) {
            onAppClicked(m_softwaremodel->index(0, 0));
        }
    }
}

const QPixmap NotificationWidget::loadSvg(const QString &fileName, const QSize &size)
{
    QPixmap pixmap(size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    return pixmap;
}

QIcon NotificationWidget::getAppIcon(const QString &appIcon, const QSize &size)
{
    const qreal ratio = devicePixelRatioF();
    QPixmap pixmap;

    QIcon icon = QIcon::fromTheme(appIcon, QIcon::fromTheme(m_theme));

    if (icon.isNull()) {
        // 有些图标是svg格式，加载
        if (appIcon.endsWith(".svg") && QFile::exists(appIcon)) {
            pixmap = loadSvg(appIcon, size * ratio);
        }
        if (!pixmap.isNull()) {
            return pixmap;
        }
        // 依然找不到，那么使用application-x-desktop代替
        icon = QIcon::fromTheme("application-x-desktop");
    }

    pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}
