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
#include "modules/defapp/defaultappsmodule.h"
#include "moduleinitthread.h"

#include "mainwindow.h"
#include "constant.h"
#include "navwinview.h"

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QDebug>
#include <QStandardItemModel>

using namespace DCC_NAMESPACE;

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
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
    m_navModel = new QStandardItemModel(m_navView);
    m_navView->setModel(m_navModel);

    connect(m_navView, &NavWinView::clicked, this, &MainWindow::onFirstItemClick);

    QTimer::singleShot(0, this, &MainWindow::initAllModule);
    QTimer::singleShot(0, this, &MainWindow::modulePreInitialize);
}

void MainWindow::initAllModule()
{
    using namespace sync;
    using namespace datetime;
    using namespace defapp;
    using namespace display;
    using namespace accounts;
    using namespace mouse;
    using namespace bluetooth;
    using namespace sound;
    using namespace personalization;

    m_modules = {
        { new AccountsModule(this), tr("Account")},
        { new SyncModule(this), tr("Cloud Sync")},
        { new DisplayModule(this), tr("Display")},
        { new DefaultAppsModule(this), tr("Default Applications")},
        { new DatetimeModule(this), tr("Time and Date")},
        { new MouseModule(this), tr("Mouse")},
        { new BluetoothModule(this), tr("Bluetooth")},
        { new SoundModule(this), tr("Sound")},
        { new PersonalizationModule(this), tr("Personalization")},
    };

    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        QStandardItem* item = new QStandardItem;
        item->setIcon(QIcon(QString(":/%1/themes/dark/icons/nav_%1.svg").arg(it->first->name())));
        item->setText(it->second);
        m_navModel->appendRow(item);
    }
}

void MainWindow::modulePreInitialize()
{
    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        it->first->preInitialize();
    }
}

void MainWindow::popWidget()
{
    QWidget *w = m_contentStack.pop().second;

    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    if (m_contentStack.isEmpty()) {//Only remain 1 level page : back to top page
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_rightView->hide();
    } else {//The second page will Covered with fill blank areas
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

//Only used to from third page to top page can use it
void MainWindow::popAllWidgets()
{
    for (int pageCount = m_contentStack.count(); pageCount > 0; pageCount--) {
        popWidget();
    }
}

void MainWindow::popWidget(ModuleInterface * const inter)
{
    Q_UNUSED(inter)

    popWidget();
}

void MainWindow::showModulePage(const QString &module, const QString &page, bool animation)
{

}

void MainWindow::setModuleVisible(ModuleInterface *const inter, const bool visible)
{
    auto find_it = std::find_if(m_modules.cbegin(),
                                m_modules.cend(),
                                [=] (const QPair<ModuleInterface*, QString>& pair) {
        return pair.first == inter;
    });

    if (find_it != m_modules.cend()) {
        m_navView->setRowHidden(find_it - m_modules.cbegin(), !visible);
        Q_EMIT moduleVisibleChanged(find_it->first->name(), visible);
    }
    else {
        qWarning() << Q_FUNC_INFO << "Not found module!";
    }
}

void MainWindow::pushWidget(ModuleInterface *const inter, QWidget *const w)
{
    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
    if (m_contentStack.size() == 2) {
        QWidget *w = m_contentStack.pop().second;
        m_rightContentLayout->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
    }

    if (m_contentStack.isEmpty()) {//Add the first second-level page, the top page changes from Icon to list (top page is not added to m_contentStack)
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_rightView->show();
    }
    else {
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    //Set the newly added page to fill the blank area
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w);
}

void MainWindow::onFirstItemClick(const QModelIndex &index)
{
    ModuleInterface* inter = m_modules[index.row()].first;

    if (!m_contentStack.isEmpty() && m_contentStack.last().first == inter) {
        return;
    }

    popAllWidgets();

    if (!m_initList.contains(inter)) {
        inter->initialize();
        m_initList << inter;
    }

    pushWidget(inter, inter->moduleWidget());
}
