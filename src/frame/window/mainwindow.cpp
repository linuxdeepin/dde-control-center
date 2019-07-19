/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#include "mainwindow.h"
#include "constant.h"
#include "navwinview.h"
#include "navigation/navmodel.h"

#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPushButton>
#include <QDebug>

#include "modules/accounts/accountsmodule.h"
#include "modules/bluetooth/bluetoothmodule.h"
#include "modules/datetime/datetimemodule.h"
#include "modules/defapp/defaultappsmodule.h"
#include "modules/keyboard/keyboardmodule.h"
#include "modules/power/powermodule.h"
#include "modules/sound/soundmodule.h"
#include "modules/update/updatemodule.h"
#include "modules/mouse/mousemodule.h"
#include "modules/wacom/wacommodule.h"
#include "modules/display/displaymodule.h"
#include "modules/personalization/personalizationmodule.h"
#include "modules/sync/syncmodule.h"
#include "modules/systeminfo/systeminfomodule.h"
#include "modules/network/networkmodule.h"
#include "moduleinitthread.h"

using namespace dcc::accounts;
using namespace dcc::datetime;
using namespace dcc::update;
using namespace dcc::cloudsync;
using namespace dcc::display;
using namespace dcc::defapp;
using namespace dcc::personalization;
using namespace dcc::network;
using namespace dcc::bluetooth;
using namespace dcc::sound;
using namespace dcc::power;
using namespace dcc::mouse;
using namespace dcc::keyboard;
using namespace dcc::sound;
using namespace dcc::wacom;
using namespace dcc::systeminfo;

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    // 初始化视图和布局结构
    QWidget *content = new QWidget(this);
    m_rightView = new QWidget(this);
    m_contentLayout = new QHBoxLayout(content);
    m_rightContentLayout = new QHBoxLayout(m_rightView);
    m_navView = new NavWinView(this);
    m_contentLayout->addWidget(m_navView);
    m_contentLayout->addWidget(m_rightView);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_rightContentLayout->setContentsMargins(10, 10, 10, 10);
    m_rightContentLayout->setSpacing(10);
    m_rightView->hide();
    m_rightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(content);

    // 初始化测试数据
    m_navModel = new NavModel(1, m_navView);
    m_navView->setModel(m_navModel);

    connect(m_navView, &NavWinView::clicked, this, &MainWindow::onFirstItemClieck);

}

void MainWindow::pushWidget(QWidget *widget)
{
    if (m_contentStack.isEmpty()) {//添加第一个第二页面，一级页面从Icon变为list (top页面不添加到m_contentStack)
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_rightView->show();
    } else {
        m_contentStack.top()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    //设置新添加进来的页面铺满空白区域
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //已经有第3级界面的时候，先移除上一个第3级页面，再添加新的第3级页面(保证只有一个3级页面)
    if (m_contentStack.size() == 2) {
        QWidget *w = m_contentStack.pop();
        m_rightContentLayout->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
    }

    m_contentStack.push(widget);
    m_rightContentLayout->addWidget(widget);
}

void MainWindow::popWidget()
{
    QWidget *w = m_contentStack.pop();

    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    if (m_contentStack.isEmpty()) {//Only remain 1 level page : back to top page
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_rightView->hide();
    } else {//The second page will Covered with fill blank areas
        m_contentStack.top()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

//Only used to from third page to top page can use it
void MainWindow::backTopPage()
{
    //pop third page
    QWidget *w = m_contentStack.pop();
    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    //pop second page
#if NOT_USE_DEMO
    QWidget *lastWidget = m_contentStack.pop();
    m_rightContentLayout->removeWidget(lastWidget);
    lastWidget->setParent(nullptr);
    lastWidget->deleteLater();
#else
    m_contentStack.pop();
    m_rightContentLayout->removeWidget(m_navSecView);
    m_navSecView->setParent(nullptr);
    m_navSecView->deleteLater();
    m_navSecModel->deleteLater();
#endif
    m_navView->setViewMode(QListView::IconMode);
    m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_rightView->hide();

}

void MainWindow::updateFirstPage(int index)//list模式，直接从一级页面的切换
{
    if (m_contentStack.count() > 1) {
        popWidget();
    } else { //只剩1个页面的时候，直接删除，但是不能返回top页面 ， 然后再new一个二级页面

        QWidget *w = m_contentStack.pop();
        m_rightContentLayout->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();

        createSecPage(1, index);

    }
}

void MainWindow::createSecPage(int count, int index)
{
    //此处根据实际情况new二级页面，下面仅为使用例子
#if NOT_USE_DEMO
    Q_UNUSED(count)

    switch (static_cast<NavModel::ModuleType>(index)) {
    case NavModel::AccountsModule:
        loadModule(new AccountsModule(this));
        break;
    case NavModel::Cloudsync:
        loadModule(new SyncModule(this));
        break;
    case NavModel::Display:
        loadModule(new DisplayModule(this));
        break;
    case NavModel::Defapp:
        loadModule(new DefaultAppsModule(this));
        break;
    case NavModel::Personalization:
        loadModule(new PersonalizationModule(this));
        break;
    case NavModel::Network:
        loadModule(new NetworkModule(this));
        break;
    case NavModel::Bluetooth:
        loadModule(new BluetoothModule(this));
        break;
    case NavModel::Sound:
        loadModule(new SoundModule(this));
        break;
    case NavModel::Datetime:
        loadModule(new DatetimeModule(this));
        break;
    case NavModel::Power:
        loadModule(new PowerModule(this));
        break;
    case NavModel::Mouse:
        loadModule(new MouseModule(this));
        break;
    case NavModel::Keyboard:
        loadModule(new KeyboardModule(this));
        break;
    case NavModel::Wacom:
        loadModule(new WacomModule(this));
        break;
    case NavModel::Update:
        loadModule(new UpdateModule(this));
        break;
    case NavModel::Systeminfo:
        loadModule(new SystemInfoModule(this));
        break;
    default:
        break;
    }

#else

    m_navSecView = new NavWinView(this);
    m_navSecView->setViewMode(QListView::ListMode);
    m_navSecView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_rightContentLayout->addWidget(m_navSecView);

    m_navSecModel = new NavModel(count, m_navSecView);
    m_navSecView->setModel(m_navSecModel);
    connect(m_navSecView, &NavWinView::clicked, this, &MainWindow::onSecondItemClieck);

    pushWidget(m_navSecView);
#endif

}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    QWidget *w = new QWidget;
    QString data = QString("background: %1").arg("red");
    w->setStyleSheet(data);
    w->setMinimumWidth(200);
    QPushButton *button = new QPushButton("pop", w);
    connect(button, &QPushButton::clicked, this, &MainWindow::popWidget);
    QPushButton *backTopBtn = new QPushButton("popTop", w);
    backTopBtn->move(0, 30);
    connect(backTopBtn, &QPushButton::clicked, this, &MainWindow::backTopPage);
    pushWidget(w);
}

void MainWindow::showModulePage(const QString &module, const QString &page, bool animation)
{

}

void MainWindow::setModuleVisible(ModuleInterface *const inter, const bool visible)
{
    // get right position to insert
    const QString name = inter->name();

    QWidget *moduleWidget = inter->moduleWidget();
    Q_ASSERT(moduleWidget);
    moduleWidget->setVisible(visible);

    Q_EMIT moduleVisibleChanged(name, visible);
}

void MainWindow::setFrameAutoHide(ModuleInterface *const inter, const bool autoHide)
{

}

void MainWindow::pushWidget(ModuleInterface *const inter, ContentWidget *const w)
{
    Q_UNUSED(inter)

    pushWidget((QWidget *)w);
}

void MainWindow::onFirstItemClieck(const QModelIndex &index)
{
    qDebug() << "onFirstItemClieck , index.row() : " << index.data(NavModel::NavModuleType).toInt();

    int type = index.data(NavModel::NavModuleType).toInt();
    if ((m_navView->viewMode() == QListView::ViewMode::ListMode)//判断当前处于top页面
            && (m_contentStack.count() == 0)) {//当一级页面为ListMode时，防止多次点击一级页面，会添加多个二级页面
        createSecPage(1, type);
    } else { //当前处于有2级页面(可能还有3级页面)，需要删除除1级页面以外的全部页面
        updateFirstPage(type);
    }

}

//Used to test
void MainWindow::onSecondItemClieck(const QModelIndex &index)
{
    qDebug() << " onSecondItemClieck , index.row() : " << index.row();

    QWidget *w = new QWidget(this);

    QString strValue = "red";
    switch (index.row()) {
    case 0:
        strValue = "red";
        break;
    case 1:
        strValue = "black";
        break;
    case 2:
        strValue = "yellow";
        break;
    case 3:
        strValue = "gray";
        break;
    case 4:
        strValue = "darkBlue";
        break;
    case 5:
        strValue = "darkCyan";
        break;
    case 6:
        strValue = "darkGreen";
        break;
    case 7:
        strValue = "darkRed";
        break;
    case 8:
        strValue = "magenta";
        break;
    case 9:
        strValue = "cyan";
        break;
    case 10:
        strValue = "blue";
        break;
    case 11:
        strValue = "lightGray";
        break;
    case 12:
        strValue = "darkGray";
        break;
    case 13:
        strValue = "darkMagenta";
        break;
    case 14:
        strValue = "darkYellow";
        break;
    case 15:
        strValue = "green";
        break;
    default:
        break;
    }

    QString data = QString("background: %1").arg(strValue);
    w->setStyleSheet(data);
    w->setMinimumWidth(200);

    QPushButton *button = new QPushButton("pop", w);
    connect(button, &QPushButton::clicked, this, &MainWindow::popWidget);
    QPushButton *backTopBtn = new QPushButton("popTop", w);
    backTopBtn->move(0, 30);
    connect(backTopBtn, &QPushButton::clicked, this, &MainWindow::backTopPage);

    pushWidget(w);
}

void MainWindow::loadModule(ModuleInterface *const module)
{
    module->initialize();
    pushWidget((QWidget *)module->moduleWidget());
}

void MainWindow::onModuleInitFinished(ModuleInterface *const module)
{
    Q_UNUSED(module)
}
