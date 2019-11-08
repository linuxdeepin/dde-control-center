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
#include "modules/commoninfo/commoninfomodule.h"
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
#include "search/searchwidget.h"
#include "dtitlebar.h"
#include "utils.h"

#include <DBackgroundGroup>
#include <DIconButton>
#include <DApplicationHelper>

#include <QHBoxLayout>
#include <QMetaEnum>
#include <QDebug>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLocale>
#include <QLinearGradient>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;
DTK_USE_NAMESPACE

//此处为带边距的宽度
const int first_widget_min_width = 188;
//此处为不带边距的宽度
const int second_widget_min_width = 230;
const int third_widget_min_width = 340;
//窗口的总宽度，带边距
const int widget_total_min_width = 820;
//当窗口宽度大于 four_widget_min_widget 时，
//四级页面将被平铺，否则会被置于顶层
const int four_widget_min_widget = widget_total_min_width + third_widget_min_width + 40;

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
    , m_searchWidget(nullptr)
    , m_firstCount(-1)
    , m_widgetName("")
    , m_backwardBtn(nullptr)
{
    //Initialize view and layout structure
    QWidget *content = new QWidget(this);

    m_contentLayout = new QHBoxLayout(content);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
    m_rightContentLayout = new QHBoxLayout();

    m_rightView = new DBackgroundGroup(m_rightContentLayout);
    m_rightView->setItemMargins(QMargins(10, 10, 10, 10));
    m_rightView->setContentsMargins(10, 10, 10, 10);

    m_navView = new DListView(this);
    m_navView->setFrameShape(QFrame::Shape::NoFrame);
    m_navView->setEditTriggers(QListView::NoEditTriggers);
    m_navView->setResizeMode(QListView::Adjust);
    m_navView->setAutoScroll(true);
    m_navView->setTabKeyNavigation(true);

    m_contentLayout->addWidget(m_navView);
    m_contentLayout->addWidget(m_rightView);

    m_rightView->hide();
    m_rightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout()->setMargin(0);
    setCentralWidget(content);

    //Initialize top page view and model
    m_navModel = new QStandardItemModel(m_navView);
    m_navView->setModel(m_navModel);
    connect(m_navView, &DListView::activated, this, &MainWindow::onFirstItemClick);
    connect(m_navView, &DListView::clicked, m_navView, &DListView::activated);

    m_searchWidget = new SearchWidget(this);
    m_searchWidget->setMinimumSize(350, 36);

    DTitlebar *titlebar = this->titlebar();
    titlebar->addWidget(m_searchWidget, Qt::AlignCenter);
    connect(m_searchWidget, &SearchWidget::notifyModuleSearch, this, &MainWindow::onEnterSearchWidget);

    m_backwardBtn = new DIconButton(this);
    m_backwardBtn->setEnabled(false);
    m_backwardBtn->setIcon(QStyle::SP_ArrowBack);
    titlebar->addWidget(m_backwardBtn, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar->setIcon(QIcon::fromTheme("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, [this] {
        //说明：只有"update"模块/"镜像源列表"页面需要从第三级页面返回第二级页面(若其他模块还有需求可以在此处添加处理)
        if (m_contentStack.last().first->name() != "update") {
            popAllWidgets();
        } else {
            popWidget();
        }
        resetNavList(m_contentStack.isEmpty());
    });

    QTimer::singleShot(0, this, &MainWindow::initAllModule);
    QTimer::singleShot(0, this, &MainWindow::modulePreInitialize);
    QTimer::singleShot(0, this, [ = ]() {
        //设置 触控板，指点杆 是否存在
        m_searchWidget->setRemoveableDeviceStatus(tr("Touchpad"), getRemoveableDeviceStatus(tr("Touchpad")));
        m_searchWidget->setRemoveableDeviceStatus(tr("TrackPoint"), getRemoveableDeviceStatus(tr("TrackPoint")));

        //after initAllModule to load ts data
        m_searchWidget->setLanguage(QLocale::system().name());
    });

    updateViewBackground();
}

void MainWindow::initAllModule()
{
    using namespace sync;
    using namespace datetime;
    using namespace defapp;
    using namespace network;
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
    using namespace commoninfo;

    m_modules = {
        { new AccountsModule(this), tr("Account")},
        { new SyncModule(this), tr("Cloud Sync")},
        { new DisplayModule(this), tr("Display")},
        { new DefaultAppsModule(this), tr("Default Applications")},
        { new PersonalizationModule(this), tr("Personalization")},
        { new NetworkModule(this), tr("Network")},
        { new SoundModule(this), tr("Sound")},
        { new BluetoothModule(this), tr("Bluetooth")},
        { new DatetimeModule(this), tr("Date and Time")},
        { new PowerModule(this), tr("Power")},
        { new MouseModule(this), tr("Mouse")},
        { new WacomModule(this), tr("Drawing Tablet")},
        { new KeyboardModule(this), tr("Keyboard and Language")},
        { new UpdateModule(this), tr("Update")},
        { new SystemInfoModule(this), tr("System Info")},
        { new CommonInfoModule(this), tr("General Settings")},
    };

    bool isIcon = m_contentStack.empty();

    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(it->first->icon());
        item->setText(it->second);

        //目前只有"update"模块需要使用右上角的角标，其他模块还是使用旧的位置数据设置
        //若其他地方需要使用右上角的角标，可在下面if处使用“||”添加对应模块的name()值
        if (it->first->name() == "update") {
            auto action = new DViewItemAction(Qt::AlignTop | Qt::AlignRight, QSize(28, 28), QSize(), true);
            action->setIcon(QIcon(":/icons/deepin/builtin/icons/dcc_common_subscript.svg"));
            action->setVisible(false);
            item->setActionList(Qt::Edge::RightEdge, {action});
            m_remindeSubscriptList.append(QPair<QString, DViewItemAction *>(it->first->name(), action));
            item->setData(QVariant::fromValue(VListViewRightSubscripItemMargin), Dtk::MarginsRole);
        } else {
            item->setData(VListViewItemMargin, Dtk::MarginsRole);
        }

        m_navModel->appendRow(item);
        m_searchWidget->addModulesName(it->first->name(), it->second);
    }

    resetNavList(isIcon);
}

void MainWindow::modulePreInitialize()
{
    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        it->first->preInitialize();

        setModuleVisible(it->first, it->first->isAvailable());
    }
}

void MainWindow::popWidget()
{
    if (m_topWidget) {
        m_topWidget->deleteLater();
        m_topWidget = nullptr;
        return;
    }

    if (!m_contentStack.size())
        return;

    QWidget *w = m_contentStack.pop().second;

    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    //delete replace widget : first delete replace widget(up code) , then pass pushWidget to set last widget
    if (m_lastThirdPage.second) {
        m_lastThirdPage.second->setVisible(true);
        pushWidget(m_lastThirdPage.first, m_lastThirdPage.second);
        //only set two pointer to nullptr , but not delete memory
        memset(&m_lastThirdPage, 0, sizeof(m_lastThirdPage));
    }

    if (m_contentStack.isEmpty())
        setCurrModule(nullptr);
}

//Only used to from third page to top page can use it
void MainWindow::popAllWidgets(int place)
{
    if (place == 0 && m_topWidget) {
        m_topWidget->deleteLater();
        m_topWidget = nullptr;
        return;
    }

    for (int pageCount = m_contentStack.count(); pageCount > place; pageCount--) {
        popWidget();
    }
}

void MainWindow::popWidget(ModuleInterface *const inter)
{
    Q_UNUSED(inter)

    popWidget();
    // modified for bug-3072 
    resetNavList(m_contentStack.isEmpty());
}

void MainWindow::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_UNUSED(animation)

    if (!isModuleAvailable(module) && calledFromDBus()) {
        qDebug() << "get error module name!";
        sendErrorReply(QDBusError::InvalidArgs,
                       "cannot find module that name is " + module);
    }

    raise();
    if (isMinimized())
        show();

    setWindowState(Qt::WindowActive | windowState());
    onEnterSearchWidget(module, page);
}

void MainWindow::setModuleSubscriptVisible(const QString &module, bool bIsDisplay)
{
    auto find_it = std::find_if(m_remindeSubscriptList.cbegin(),
                                m_remindeSubscriptList.cend(),
    [ = ](const QPair<QString, DViewItemAction *> &pair) {
        return pair.first == module ? pair.second : nullptr;
    });

    if (find_it->first != module || find_it == m_remindeSubscriptList.cend())
        return;

    if (find_it->second->isVisible() != bIsDisplay) {
        find_it->second->setVisible(bIsDisplay);
        m_navView->update();
    }
}

bool MainWindow::isModuleAvailable(const QString &m)
{
    for (auto ite : m_modules) {
        if (ite.first->name() == m) {
            return ite.first->isAvailable();
        }
    }

    if (calledFromDBus()) {
        qDebug() << "get error module name!";
        sendErrorReply(QDBusError::InvalidArgs,
                       "cannot find module that name is " + m);
    }

    return false;
}

void MainWindow::toggle()
{
    raise();
    if (isMinimized())
        show();

    setWindowState(Qt::WindowActive | windowState());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);

    auto dstWidth = event->size().width();
    if (four_widget_min_widget > dstWidth ) {
        //update只有当Mirror页面存在时，才会有第二级页面；size变化小了，需要恢复成topWidget
        if ((m_contentStack.size() == 3) || (m_contentStack.size() == 2 && m_contentStack.first().first->name() == "update")) {
            auto ite = m_contentStack.pop();
            pushTopWidget(ite.first, ite.second);
        } else {
            qWarning() << "Not satisfied , can't back.";
        }
    } else if (four_widget_min_widget <= dstWidth) {
        if (!m_topWidget) {
            qWarning() << " The top widget is nullptr.";
            return;
        }

        auto layout = m_topWidget->layout();
        int idx = 0;
        while (auto itemwidget = layout->itemAt(idx)) {
            if (itemwidget->widget() == m_topWidget->curWidget()) {
                layout->takeAt(idx);
                break;
            }
            ++idx;
        }
        pushFinalWidget(m_topWidget->curInterface(), m_topWidget->curWidget());

        m_topWidget->deleteLater();
        m_topWidget = nullptr;
    }

    if (m_topWidget) {
        m_topWidget->setFixedSize(event->size());
        m_topWidget->curWidget()->setMinimumWidth(dstWidth / 2);
        m_topWidget->setFixedHeight(height() - this->titlebar()->height());
    }
}

void MainWindow::resetNavList(bool isIconMode)
{
    if (isIconMode && m_navView->viewMode() == QListView::IconMode)
        return;

    if (!isIconMode && m_navView->viewMode() == QListView::ListMode)
        return;

    if (isIconMode) {
        //Only remain 1 level page : back to top page
        m_navView->setViewportMargins(QMargins(0, 0, 0, 0));
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setDragEnabled(false);
        m_navView->setMaximumWidth(QWIDGETSIZE_MAX);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_navView->setIconSize(QSize(84, 84));
        m_navView->setItemSize(QSize(170, 168));
        m_navView->setItemSpacing(0);
        m_navView->setSpacing(20);
        m_navView->clearSelection();
        m_navView->setSelectionMode(QAbstractItemView::NoSelection);

        //Icon模式，"update"使用右上角角标Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if(m_modules.at(i).first->name() == data.first) {
                    m_navModel->item(i, 0)->setData(QVariant::fromValue(VListViewRightSubscripItemMargin), Dtk::MarginsRole);
                    break;
                }
            }
        }

        m_rightView->hide();
        m_backwardBtn->setEnabled(false);
    } else {
        //The second page will Covered with fill blank areas
        m_navView->setViewportMargins(QMargins(10, 0, 10, 0));
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setFixedWidth(first_widget_min_width);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_navView->setIconSize(QSize(42, 42));
        m_navView->setItemSize(QSize(168, 48));
        m_navView->setSpacing(0);
        m_navView->setSelectionMode(QAbstractItemView::SingleSelection);

        //List模式，"update"使用统一Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if(m_modules.at(i).first->name() == data.first) {
                    m_navModel->item(i, 0)->setData(QVariant::fromValue(VListViewItemMargin), Dtk::MarginsRole);
                    break;
                }
            }
        }

        // 选中当前的项
        m_navView->selectionModel()->select(m_navView->currentIndex(), QItemSelectionModel::SelectCurrent);
        m_rightView->show();
        m_backwardBtn->setEnabled(true);
        m_contentStack.top().second->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    m_navView->setAutoFillBackground(!isIconMode);
    updateViewBackground();
}

void MainWindow::onEnterSearchWidget(QString moduleName, QString widget)
{
    qDebug() << Q_FUNC_INFO << " moduleName : " << moduleName << " , widget :" << widget;

    if (!m_contentStack.isEmpty()
            && (m_contentStack.top().first->name() == moduleName)
            && (m_widgetName == widget)) {
        qDebug() << Q_FUNC_INFO << " Search widget is current display widget.";
        return;
    }

    for (int firstCount = 0; firstCount < m_modules.count(); firstCount++) {
        //Compare moduleName and m_modules.second(module name)
        if (moduleName == m_modules[firstCount].first->name()) {            
            //enter first level widget
            m_navView->setCurrentIndex(m_navView->model()->index(firstCount, 0));
            onFirstItemClick(m_navView->model()->index(firstCount, 0));

            //当从dbus搜索进入这里时，如果传入的page参数错误，则会使用m_widgetName保存一个错的数据。
            //这样当前界面实际有进入的模块的界面界面。这样会出现重复调用该函数显示同一个界面的情况
            //目前看对程序没有影响
            m_firstCount = firstCount;
            m_widgetName = widget;

            //notify related module load widget
//            QTimer::singleShot(0, this, [ = ] { //avoid default and load sequence in time
            auto errCode = m_modules[m_firstCount].first->load(m_widgetName);
            if (!errCode || m_widgetName == "") {
                return;
            }

            auto errStr = QString("on module %1, cannot search page %2!")
                            .arg(moduleName, m_widgetName);
            qDebug() << errStr;

            if (calledFromDBus()) {
                sendErrorReply(QDBusError::InvalidArgs, errStr);
            }
//            });
            break;
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateViewBackground();

        if (m_topWidget) {
            m_topWidget->update();
        }
    }

    return DMainWindow::changeEvent(event);
}

void MainWindow::setModuleVisible(ModuleInterface *const inter, const bool visible)
{
    inter->setAvailable(visible);

    auto find_it = std::find_if(m_modules.cbegin(),
                                m_modules.cend(),
    [ = ](const QPair<ModuleInterface *, QString> &pair) {
        return pair.first == inter;
    });

    if (find_it != m_modules.cend()) {
        m_navView->setRowHidden(find_it - m_modules.cbegin(), !visible);
        Q_EMIT moduleVisibleChanged(find_it->first->name(), visible);

        qDebug() << "[SearchWidget] find_it->first->name() : " << find_it->first->name() << visible;
        if ("bluetooth" == find_it->first->name()) {
            if (visible) {
                m_searchWidget->removeUnExsitData(tr("Bluetooth"));
            } else {
                m_searchWidget->addUnExsitData(tr("Bluetooth"));

                //当前处于＂蓝牙＂页面才会回到主页面
                if (m_contentStack.count() > 0 && m_contentStack.at(0).first->name() == "bluetooth") {
                    popAllWidgets();
                    resetNavList(m_contentStack.empty());
                }
            }
        } else if ("wacom" == find_it->first->name()) {
            if (visible) {
                m_searchWidget->removeUnExsitData(tr("Drawing Tablet"));
            } else {
                m_searchWidget->addUnExsitData(tr("Drawing Tablet"));

                //当前处于＂数位板＂页面才会回到主页面
                if (m_contentStack.count() > 0 && m_contentStack.at(0).first->name() == "wacom") {
                    popAllWidgets();
                    resetNavList(m_contentStack.empty());
                }
            }
        }  else if ("cloudsync" == find_it->first->name()) {
            if (visible) {
                m_searchWidget->removeUnExsitData(tr("Cloud Sync"));
            } else {
                m_searchWidget->addUnExsitData(tr("Cloud Sync"));
            }
        }
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
    case CoverTop://根据当前页面宽度去计算新增的页面放在最后面一层，或者Top页面
        judgeTopWidgetPlace(inter, w);
        break;
    case DirectTop://不需要管页面宽度，直接将新增页面放在Top页面；为解决某些页面使用CoverTop无法全部展示的问题
        pushTopWidget(inter, w);
        break;
    case Normal:
    default:
        pushNormalWidget(inter, w);
        break;
    }

    if (m_contentStack.size() > 0) {
        setTabOrder(this->titlebar(), m_navView);
        m_navView->setFocus();

        QWidget *secondWidget = qobject_cast<QWidget *>(m_contentStack.at(0).second);
        if (secondWidget != nullptr) {
            setTabOrder(m_navView, secondWidget);
        }
        if (m_contentStack.size() > 1) {
            QWidget *thirdWidget = qobject_cast<QWidget *>(m_contentStack.at(1).second);
            if (thirdWidget != nullptr) {
                setTabOrder(secondWidget, thirdWidget);
                setTabOrder(thirdWidget, this->titlebar());
            }
        }
    }
}

//First save the third level page, Then pop the third level page
//Next set the new page as the third level page,
//Finally when in the new third level page clicked "pop" button , return to the old three level page
void MainWindow::replaceThirdWidget(ModuleInterface *const inter, QWidget *const w)
{
    if (m_contentStack.count() != 2)
        return;

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
    Q_UNUSED(inter)

    m_topWidget = new FourthColWidget(this);
    m_topWidget->initWidget(w, inter);
    m_topWidget->setVisible(true);

    m_topWidget->setFixedHeight(height() - this->titlebar()->height());
    m_topWidget->move(0, titlebar()->height());
    resetNavList(m_contentStack.empty());
}

//为后面平铺模式留接口
void MainWindow::pushFinalWidget(ModuleInterface *const inter, QWidget *const w)
{
    w->layout()->setMargin(0);
    w->setContentsMargins(0, 0, 0, 0);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w, 7);

    w->setMinimumWidth(third_widget_min_width);
    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);
    }
}

void MainWindow::pushNormalWidget(ModuleInterface *const inter, QWidget *const w)
{
    //When there is already a third-level page, first remove the previous third-level page,
    //then add a new level 3 page (guaranteed that there is only one third-level page)
    popAllWidgets(1);
    //Set the newly added page to fill the blank area
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_contentStack.push({inter, w});
    m_rightContentLayout->addWidget(w, m_contentStack.size() == 1 ? 3 : 7);

    if (m_contentStack.size() == 2) {
        m_contentStack.at(0).second->setMinimumWidth(second_widget_min_width);
        m_contentStack.at(1).second->setMinimumWidth(third_widget_min_width);
    }

    resetNavList(m_contentStack.empty());
}

void MainWindow::judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w)
{
    int totalWidth = m_navView->minimumWidth() + 20;//10 is first and second widget space　, other 10 is last widget with right box space
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
        if (four_widget_min_widget < width()) {
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

void MainWindow::updateViewBackground()
{
    if (m_navView->viewMode() == QListView::IconMode) {
        DPalette pa = DApplicationHelper::instance()->palette(m_navView);
        QColor base_color = palette().base().color();
        DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(base_color);

        if (ct == DGuiApplicationHelper::LightType) {
            pa.setBrush(DPalette::ItemBackground, palette().base());
        } else {
            base_color = DGuiApplicationHelper::adjustColor(base_color, 0, 0, +5, 0, 0, 0, 0);
            pa.setColor(DPalette::ItemBackground, base_color);
        }

        DApplicationHelper::instance()->setPalette(m_navView, pa);
    } else {
        DPalette pa = DApplicationHelper::instance()->palette(m_navView);
        pa.setColor(DPalette::ItemBackground, Qt::transparent);

        DApplicationHelper::instance()->setPalette(m_navView, pa);
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
    setCurrModule(inter);
    inter->active();
}

FourthColWidget::FourthColWidget(QWidget *parent)
    : QWidget(parent)
{
}

void FourthColWidget::initWidget(QWidget *showWidget, ModuleInterface *module)
{
    this->setMinimumSize(parentWidget()->width(), this->parentWidget()->height());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    showWidget->setMinimumWidth(this->parentWidget()->width() / 2);
    showWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    showWidget->setAutoFillBackground(true);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch(7);
    layout->addWidget(showWidget, 4, Qt::AlignRight);
    this->setLayout(layout);
    m_curWidget = showWidget;
    m_curInterface = module;

    update();
}

void FourthColWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QLinearGradient linear;
    int startPosX = pos().x() + m_curWidget->x();
    linear.setStart(QPointF(startPosX, 0));
    linear.setFinalStop(QPointF(startPosX - 30, 0));
    if (DGuiApplicationHelper::LightType == DApplicationHelper::instance()->themeType()) {
        linear.setColorAt(0, QColor(0, 0, 0, 13));
    } else {
        linear.setColorAt(0, QColor(0, 0, 0, 26));
    }

    linear.setColorAt(1, QColor(0, 0, 0, 0));

    DPalette pa = DApplicationHelper::instance()->palette(this);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    QColor curThemeColor = pa.base().color();
    curThemeColor.setAlphaF(0.6);  // 设置透明度
    painter.setBrush(QBrush(QColor(curThemeColor)));
    painter.drawRect(this->parentWidget()->rect());

    painter.setBrush(QBrush(linear));

    QRect rt(startPosX, 0, -30, rect().height());
    painter.drawRect(rt);
}

void MainWindow::setRemoveableDeviceStatus(QString type, bool state)
{
    //state:true,list里面不存在，则加入到list
    //state:false,list里面存在，则从list移除
    if (state) {
        if (!m_removeableDeviceList.contains(type)) {
            m_removeableDeviceList.append(type);
        }
    } else {
        if (m_removeableDeviceList.contains(type)) {
            m_removeableDeviceList.removeOne(type);
        }
    }

    //通知设备“添加/移除"状态
    if (m_searchWidget)
        m_searchWidget->setRemoveableDeviceStatus(type, state);
}

bool MainWindow::getRemoveableDeviceStatus(QString type) const
{
    return m_removeableDeviceList.contains(type);
}
