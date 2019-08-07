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
#include "modules/update/mirrorswidget.h"

#include "mainwindow.h"
#include "constant.h"
#include "navwinview.h"

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QDebug>
#include <QStandardItemModel>
#include <QPushButton>

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
    using namespace power;
    using namespace update;
    using namespace keyboard;
    using namespace wacom;

    m_modules = {
        { new AccountsModule(this), tr("Account")},
        { new SyncModule(this), tr("Cloud Sync")},
        { new DisplayModule(this), tr("Display")},
        { new DefaultAppsModule(this), tr("Default Applications")},
        { new DatetimeModule(this), tr("Time and Date")},
        { new MouseModule(this), tr("Mouse")},
        { new KeyboardModule(this), tr("Keyboard")},
        { new BluetoothModule(this), tr("Bluetooth")},
        { new SoundModule(this), tr("Sound")},
        { new PersonalizationModule(this), tr("Personalization")},
        { new PowerModule(this), tr("Power")},
        { new UpdateModule(this), tr("Update")},
        { new WacomModule(this), tr("Wacom")},
    };

    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        QStandardItem *item = new QStandardItem;
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

    memset(&m_lastThirdPage, 0, sizeof(m_lastThirdPage));
}

void MainWindow::popWidget(ModuleInterface *const inter)
{
    Q_UNUSED(inter)

    popWidget();
}

void MainWindow::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_UNUSED(module)
    Q_UNUSED(page)
    Q_UNUSED(animation)
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (m_topPage) {
        m_topPage->setFixedSize(event->size());
    }
}

void MainWindow::setModuleVisible(ModuleInterface *const inter, const bool visible)
{
    auto find_it = std::find_if(m_modules.cbegin(),
                                m_modules.cend(),
    [ = ](const QPair<ModuleInterface *, QString> &pair) {
        return pair.first == inter;
    });

    if (find_it != m_modules.cend()) {
        m_navView->setRowHidden(find_it - m_modules.cbegin(), !visible);
        Q_EMIT moduleVisibleChanged(find_it->first->name(), visible);
    } else {
        qWarning() << Q_FUNC_INFO << "Not found module!";
    }
}

void MainWindow::pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type)
{
    switch (type) {
    case Replace:
        replaceThirdWidget(inter, w);
        break;
    case CoverTop:
        pushTopWidget(inter, w);
        break;
    case Normal:
    default:
        pushNormalWidget(inter, w);
        break;
    }
}

//First save the third level page, Then pop the third level page
//Next set the new page as the third level page,
//Finally when in the new third level page clicked "pop" button , return to the old three level page
void MainWindow::replaceThirdWidget(ModuleInterface *const inter, QWidget *const w)
{
    if (m_contentStack.count() != 2)    return;

    //if need pop the replace widget and set old widget : link the function of slotfunc
    auto slotfunc = [ = ]() {
        popWidget();

        if (m_lastThirdPage.second) {
            m_lastThirdPage.second->setVisible(true);
            pushNormalWidget(m_lastThirdPage.first, m_lastThirdPage.second);
        }

        memset(&m_lastThirdPage, 0, sizeof(m_lastThirdPage));
    };
    Q_UNUSED(slotfunc)

    QPair<ModuleInterface *, QWidget *>widget = m_contentStack.pop();
    m_lastThirdPage.first = widget.first;
    m_lastThirdPage.second = widget.second;
    m_lastThirdPage.second->setVisible(false);

    w->setParent(m_lastThirdPage.second);//the replace widget follow the old third widget to delete
    pushNormalWidget(inter, w);
}

void MainWindow::pushTopWidget(ModuleInterface *const inter, QWidget *const w)
{
    QWidget *topWidget = new QWidget;
    QPalette pe;
    pe.setColor(QPalette::Background, QColor(238, 238, 238, 51));
    topWidget->setPalette(pe);
    topWidget->setFixedSize(this->width(), this->height());

    QHBoxLayout *layout = new QHBoxLayout;
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    pe.setColor(QPalette::Background, QColor(0, 0, 0, 255));
    w->setPalette(pe);

    linkTopBackSignal(inter->name(), w);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(w, 0, Qt::AlignRight);
    topWidget->setLayout(layout);

    topWidget->setParent(this);
    topWidget->setVisible(true);

    m_topPage = topWidget;
}

void MainWindow::linkTopBackSignal(QString moduleName, QWidget *w)
{
    //link update::MirrorsWidget backButton
    if (moduleName == tr("update")) {
        DCC_NAMESPACE::update::MirrorsWidget *widget = dynamic_cast<DCC_NAMESPACE::update::MirrorsWidget *>(w);
        connect(widget, &DCC_NAMESPACE::update::MirrorsWidget::notifyBackpage, this, [&]() {
            if (m_topPage) {
                m_topPage->deleteLater();
                m_topPage = nullptr;
            }
        });
    }

    //if some module need from topRight widget back normal widget need imitate up
}

void MainWindow::onFirstItemClick(const QModelIndex &index)
{
    ModuleInterface *inter = m_modules[index.row()].first;

    if (!m_contentStack.isEmpty() && m_contentStack.last().first == inter) {
        return;
    }

    popAllWidgets();

    if (!m_initList.contains(inter)) {
        inter->initialize();
        m_initList << inter;
    }

    pushNormalWidget(inter, inter->moduleWidget());
}

void MainWindow::pushNormalWidget(ModuleInterface *const inter, QWidget *const w)
{
    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
    if (m_contentStack.size() == 2) {
        QWidget *widget = m_contentStack.pop().second;
        m_rightContentLayout->removeWidget(widget);
        widget->setParent(nullptr);
        widget->deleteLater();
    }

    if (m_contentStack.isEmpty()) {//Add the first second-level page, the top page changes from Icon to list (top page is not added to m_contentStack)
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_rightView->show();
    } else {
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    //Set the newly added page to fill the blank area
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w);
}
