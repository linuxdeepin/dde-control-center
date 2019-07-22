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

#include "mainwindow.h"
#include "constant.h"
#include "navwinview.h"

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QDebug>

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
    , m_navModelType(NavModel::ModuleType::Default)
{
    //Initialize view and layout structure
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

    //Initialize top page view and model
    m_navModel = new NavModel(1, m_navView);
    m_navView->setModel(m_navModel);

    connect(m_navView, &NavWinView::clicked, this, &MainWindow::onFirstItemClieck);
}

void MainWindow::pushWidget(QWidget *widget)
{
    if (m_contentStack.isEmpty()) {//Add the first second-level page, the top page changes from Icon to list (top page is not added to m_contentStack)
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_rightView->show();
    } else {
        m_contentStack.top()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    //Set the newly added page to fill the blank area
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
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

void MainWindow::updateFirstPage(int index)//When in listMode，Dirct change page from first-level
{
    if (m_contentStack.count() > 1) {
        popWidget();
    } else { //When there is only 1 page left, delete it directly, but can't return to the top page, Then new one second page
        if (m_navModelType != static_cast<NavModel::ModuleType>(index)) {
            m_navModelType = static_cast<NavModel::ModuleType>(index);

            QWidget *w = m_contentStack.pop();
            m_rightContentLayout->removeWidget(w);
            w->setParent(nullptr);
            w->deleteLater();
            createSecPage(1, index);
        } else {
            qDebug() << " Request the same type : " << QMetaEnum::fromType<NavModel::ModuleType>().valueToKey(index);
        }
    }
}

void MainWindow::createSecPage(int count, int index)
{
    //According to actual click index to load module
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
    QString data("background: red");
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
    qDebug() << "onFirstItemClieck , index.row() : " << QMetaEnum::fromType<NavModel::ModuleType>().valueToKey(index.row());

    int type = index.data(NavModel::NavModuleType).toInt();
    if ((m_navView->viewMode() == QListView::ViewMode::ListMode)//Judge the current page is top page
            && (m_contentStack.count() == 0)) {//When the first-level page is ListMode, it prevents multiple clicks on the first-level page Will add multiple secondary pages
        createSecPage(1, type);
    } else { //Currently in second-level(or third-level), you need to delete all pages except the first-level page.
        updateFirstPage(type);
    }
}

#if !NOT_USE_DEMO
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
#endif

void MainWindow::loadModule(ModuleInterface *const module)
{
    module->initialize();
    pushWidget((QWidget *)module->moduleWidget());
}
