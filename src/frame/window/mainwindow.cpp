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
    , m_contentLayout(nullptr)
    , m_rightContentLayout(nullptr)
    , m_navView(nullptr)
    , m_rightView(nullptr)
    , m_navModel(nullptr)
    , m_bIsFinalWidget(false)
    , m_bIsFromSecondAddWidget(false)
    , m_topWidget(nullptr)
{
    //Initialize view and layout structure
    QWidget *content = new QWidget(this);
    m_rightView = new QWidget(this);

    m_contentLayout = new QHBoxLayout(content);
    m_rightContentLayout = new QHBoxLayout(m_rightView);
    m_navView = new NavWinView(this);
    m_navView->setFrameShape(QFrame::Shape::NoFrame);


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
    m_navView->setMinimumWidth(first_widget_min_width);
    m_navView->setViewMode(QListView::IconMode);
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
    using namespace systeminfo;

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
        { new SystemInfoModule(this), tr("Systeminfo")},
    };

    auto dele = static_cast<NavDelegate *>(m_navView->itemDelegate());
    bool isIcon = dele->viewMode() == QListView::IconMode;

    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        QStandardItem *item = new QStandardItem;
        item->setIcon(QIcon(QString(":/%1/themes/dark/icons/nav_%1.svg").arg(it->first->name())));
        item->setText(it->second);
        item->setData(isIcon ? Dtk::RoundedBackground : QVariant(),
                      Dtk::BackgroundTypeRole);
        item->setSizeHint(isIcon ? QSize(170, 168) : QSize(168, 48));
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
void MainWindow::popAllWidgets(int place)
{
    for (int pageCount = m_contentStack.count(); pageCount > place; pageCount--) {
        popWidget();
    }
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
    if (m_topWidget) {
        m_topWidget->setFixedSize(event->size());
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
    if (!inter)  {
        qWarning() << Q_FUNC_INFO << " inter is nullptr";
        return;
    }

    if (!w)  {
        qWarning() << Q_FUNC_INFO << " widget is nullptr";
        return;
    }

    switch (type) {
    case Replace:
        replaceThirdWidget(inter, w);
        break;
    case CoverTop:
        judgeTopWidgetPlace(inter, w);
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

    linkReplaceBackSignal(inter->name(), w);

    if (m_lastThirdPage.second) {
        memset(&m_lastThirdPage, 0, sizeof(m_lastThirdPage));
    }

    QPair<ModuleInterface *, QWidget *>widget = m_contentStack.pop();
    m_lastThirdPage.first = widget.first;
    m_lastThirdPage.second = widget.second;
    m_lastThirdPage.second->setVisible(false);

    w->setParent(m_lastThirdPage.second);//the replace widget follow the old third widget to delete
    m_lastThirdPage.second->setParent(m_contentStack.top().second);
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

    if (m_topWidget) {
        m_topWidget->deleteLater();
        m_topWidget = nullptr;
    }

    m_topWidget = topWidget;
}

void MainWindow::pushFinalWidget(ModuleInterface *const inter, QWidget *const w)
{
    QPalette pe;
    pe.setColor(QPalette::Background, QColor(0, 0, 0, 255));
    w->setPalette(pe);
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    linkTopBackSignal(inter->name(), w);//link back signal

    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w);

    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);
    }
}

void MainWindow::linkReplaceBackSignal(QString moduleName, QWidget *w)
{
    //if need pop the replace widget and set old widget : link the function of slotfunc
    auto slotfunc = [ = ]() {
        popWidget();

        if (m_lastThirdPage.second) {
            m_lastThirdPage.second->setVisible(true);
            pushWidget(m_lastThirdPage.first, m_lastThirdPage.second);
        }

        memset(&m_lastThirdPage, 0, sizeof(m_lastThirdPage));
    };

    Q_UNUSED(slotfunc)
    Q_UNUSED(moduleName)
    Q_UNUSED(w)

    //link widget backButton , For example like down place
//    if (moduleName == tr("xxx")) {
//        DCC_NAMESPACE::xxx::xxxWidget *widget = dynamic_cast<DCC_NAMESPACE::xxx::xxxWidget *>(w);
//        connect(widget, &DCC_NAMESPACE::xxx::xxxWidget::notifyBackpage, this, slotDeletefunc);
//    }
}

void MainWindow::linkTopBackSignal(QString moduleName, QWidget *w)
{
    auto slotDeletefunc = [ = ]() {
        if (m_bIsFinalWidget) {
            popWidget();
        } else {
            if (m_topWidget) {
                m_topWidget->deleteLater();
                m_topWidget = nullptr;
            }
        }
    };

    //link update::MirrorsWidget backButton
    if (moduleName == tr("update")) {
        DCC_NAMESPACE::update::MirrorsWidget *widget = dynamic_cast<DCC_NAMESPACE::update::MirrorsWidget *>(w);
        connect(widget, &DCC_NAMESPACE::update::MirrorsWidget::notifyBackpage, this, slotDeletefunc);
    }

    //if some module need from topRight widget back normal widget need imitate up
}

void MainWindow::judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w)
{
    int totalWidth = m_navView->minimumWidth() + 10;//10 is first and second widget space
    int contentCount = m_contentStack.count();

    if (m_bIsFinalWidget) {
        m_bIsFinalWidget = false;
    }

    for (int count = 0; count < contentCount; count++) {
        totalWidth += m_contentStack.at(count).second->minimumWidth();
    }

    //according current content widgets count to calculate use top widget or right widget
    switch (contentCount) {
    case 1: //from second widget to add top/right widget (like update setting : source list)
        if (totalWidth < widget_total_min_width) {
            m_bIsFinalWidget = true;
            m_bIsFromSecondAddWidget = true;//save from pushWidget of CoverTop type to add final(right) widget
        }
        break;
    case 2: //from third widget to add top/right widget
        if (totalWidth < widget_total_min_width) {
            m_bIsFinalWidget = true;
        }

        if (m_bIsFromSecondAddWidget) {
            popAllWidgets(1);//pop the final widget, need distinguish
            m_bIsFromSecondAddWidget = false;
        }
        break;
    case 3: //replace final widget to new top/right widget
        m_bIsFinalWidget = true;
        popAllWidgets(2);//move fourth widget(m_navView not in it , other level > 2)
        break;
    default:
        qWarning() << Q_FUNC_INFO << " error widget content conut : " << contentCount;
        return;
    }

    if (m_bIsFinalWidget) {
        pushFinalWidget(inter, w);
    } else {
        pushTopWidget(inter, w);
    }
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
    inter->active();
}

void MainWindow::pushNormalWidget(ModuleInterface *const inter, QWidget *const w)
{
    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
    popAllWidgets(1);

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

    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);
    }
}
