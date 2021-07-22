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
#include "modules/unionid/unionidmodule.h"
#include "modules/sound/soundmodule.h"
#include "modules/update/updatemodule.h"
#include "modules/mouse/mousemodule.h"
#include "modules/wacom/wacommodule.h"
#include "modules/display/displaymodule.h"
#include "modules/touchscreen/touchscreenmodule.h"
#include "modules/personalization/personalizationmodule.h"
#include "modules/sync/syncmodule.h"
#include "modules/notification/notificationmodule.h"
#include "modules/systeminfo/systeminfomodule.h"
#include "modules/network/networkmodule.h"
#include "modules/defapp/defaultappsmodule.h"
#include "modules/update/mirrorswidget.h"
#include "widgets/multiselectlistview.h"
#include "mainwindow.h"
#include "insertplugin.h"
#include "constant.h"
#include "search/searchwidget.h"
#include "dtitlebar.h"
#include "utils.h"
#include "interface/moduleinterface.h"
#include "window/gsettingwatcher.h"

#include <DBackgroundGroup>
#include <DIconButton>
#include <DApplicationHelper>
#include <DSlider>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QMetaEnum>
#include <QDebug>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLocale>
#include <QLinearGradient>
#include <QGSettings>
#include <QScroller>
#include <QScreen>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDialog>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::search;
DTK_USE_NAMESPACE
#define GSETTINGS_HIDE_MODULE "hide-module"
#define GSETTINGS_HIDE_VERSIONTYPR "hide-version-type-module"

const QByteArray ControlCenterGSettings = "com.deepin.dde.control-center";
const QString GSettinsWindowWidth = "window-width";
const QString GSettinsWindowHeight = "window-height";
const QString ModuleDirectory = "/usr/lib/dde-control-center/modules";

static int WidgetMinimumWidth = 820;
static int WidgetMinimumHeight = 634;

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

const QMargins navItemMargin(5, 3, 5, 3);
const QVariant NavItemMargin = QVariant::fromValue(navItemMargin);

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
    , m_lastSize(WidgetMinimumWidth, WidgetMinimumHeight)
{
    //Initialize view and layout structure
    DMainWindow::installEventFilter(this);

    QWidget *content = new QWidget(this);
    content->setObjectName("contentwindow");
    m_contentLayout = new QHBoxLayout(content);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
    m_rightContentLayout = new QHBoxLayout();
    m_rightContentLayout->setContentsMargins(0, 0, 0, 0);

    m_rightView = new DBackgroundGroup(m_rightContentLayout);
    m_rightView->setObjectName("modulepage");
    m_rightView->setItemSpacing(2);
    m_rightView->setContentsMargins(10, 10, 10, 10);

    m_navView = new dcc::widgets::MultiSelectListView(this);
    m_navView->setAccessibleName("Form_mainmenulist");
    m_navView->setFrameShape(QFrame::Shape::NoFrame);
    m_navView->setEditTriggers(QListView::NoEditTriggers);
    m_navView->setResizeMode(QListView::Adjust);
    m_navView->setAutoScroll(true);

    QScroller::grabGesture(m_navView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_navView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_contentLayout->addWidget(m_navView, 1);
    m_contentLayout->addWidget(m_rightView, 5);

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
    m_searchWidget->setAccessibleName("SearchModule");
    m_searchWidget->lineEdit()->setAccessibleName("SearchModuleLineEdit");
    GSettingWatcher::instance()->bind("mainwindowSearchEdit", m_searchWidget);

    DTitlebar *titlebar = this->titlebar();
    auto widhetlist = titlebar->children();
    for (auto child : widhetlist) {
        if (auto item = qobject_cast<DIconButton *>(child)) {
            item->setAccessibleName("FrameIcom");
        }
    }
    titlebar->setAccessibleName("Mainwindow bar");
    titlebar->addWidget(m_searchWidget, Qt::AlignCenter);
    connect(m_searchWidget, &SearchWidget::notifyModuleSearch, this, &MainWindow::onEnterSearchWidget);

    auto menu = titlebar->menu();
    if (!menu) {
        qDebug() << "menu is nullptr, create menu!";
        menu = new QMenu;
    } else {
        qDebug() << "get title bar menu :" << menu;
    }
    menu->setAccessibleName("titlebarmenu");
    titlebar->setMenu(menu);

    auto action = new QAction(tr("Help"));
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [ = ] {
        openManual();
    });

    m_backwardBtn = new DIconButton(this);
    m_backwardBtn->setAccessibleName("backwardbtn");
    m_backwardBtn->setEnabled(false);
    m_backwardBtn->setIcon(QStyle::SP_ArrowBack);
    QWidget *backWidget = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addSpacing(5);
    btnLayout->addWidget(m_backwardBtn);
    backWidget->setLayout(btnLayout);
    titlebar->addWidget(backWidget, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar->setIcon(QIcon::fromTheme("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, [this] {
        //说明：只有"update"模块/"镜像源列表"页面需要从第三级页面返回第二级页面(若其他模块还有需求可以在此处添加处理)
        if (!m_contentStack.isEmpty() && m_contentStack.last().first->name() != "update") {
            popAllWidgets();
        } else {
            popWidget();
        }
        m_moduleName = "";
        resetNavList(m_contentStack.isEmpty());
    });
    updateViewBackground();
    updateWinsize();
}

MainWindow::~MainWindow()
{
    QGSettings gs(ControlCenterGSettings, QByteArray(), this);
    gs.set(GSettinsWindowWidth, width());
    gs.set(GSettinsWindowHeight, height());

    qDebug() << "~MainWindow";
    for (auto m : m_modules) {
        if (m.first) {
            delete m.first;
        }
    }

    QScroller *scroller = QScroller::scroller(m_navView->viewport());
    if (scroller) {
        scroller->stop();
    }
}

void MainWindow::onBack()
{
    popWidget();
}

void MainWindow::resetTabOrder()
{
    QWidget *pre = m_navView;
    pre->setFocusPolicy(Qt::TabFocus);
    for (int i = 0; i < m_contentStack.size(); ++i) {
        auto tw = m_contentStack.at(i).second;
        findFocusChild(tw, pre);
    }
}

void MainWindow::findFocusChild(QWidget *w, QWidget *&pre)
{
    if (!w->layout() || w->layout()->count() == 0)
        return;

    auto l = w->layout();
    findFocusChild(l, pre);
}

void MainWindow::findFocusChild(QLayout *l, QWidget *&pre)
{
    for (int i = 0; i < l->count(); ++i) {
        auto cw = l->itemAt(i)->widget();
        auto cl = l->itemAt(i)->layout();

        if (cw) {
            if (cw && cw->layout() && cw->layout()->count()) {
                findFocusChild(cw, pre);
            }

            if (auto scr = qobject_cast<QScrollArea *>(cw)) {
                if (scr->widget()) {
                    findFocusChild(scr->widget(), pre);
                }
            }

            if ((!qobject_cast<QAbstractButton *>(cw)
                    && !qobject_cast<QLineEdit *>(cw)
                    && !qobject_cast<QAbstractItemView *>(cw)
                    && !qobject_cast<QAbstractSlider *>(cw)) || !cw->isEnabled()) {
                continue;
            }

            if (!pre) {
                pre = cw;
                continue;
            }

            setTabOrder(pre, cw);
            pre = cw;
        } else if (cl) {
            if (cl && cl->layout() && cl->layout()->count()) {
                findFocusChild(cl, pre);
            }
        }
    }
}

void MainWindow::initAllModule(const QString &m)
{
    if (m_bInit)
        return;

    m_bInit = true;
    using namespace sync;
    using namespace unionid;
    using namespace datetime;
    using namespace defapp;
    using namespace network;
    using namespace display;
    using namespace touchscreen;
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
    using namespace notification;

    QString idType = "Union ID";
    if (DSysInfo::isCommunityEdition())
        idType = "Deepin ID";

    m_modules = {
        { new AccountsModule(this), tr("Accounts")},
        // 原union ID 暂时隐藏
        // { new UnionidModule(this), "Union ID"},
        //~ contents_path /cloudsync/Cloud Sync
        { new SyncModule(this), DSysInfo::isCommunityEdition() ? "Deepin ID" : "Union ID"},
        { new DisplayModule(this), tr("Display")},
        { new TouchscreenModule(this), tr("Touch Screen")},
        { new DefaultAppsModule(this), tr("Default Applications")},
        { new PersonalizationModule(this), tr("Personalization")},
        { new NetworkModule(this), tr("Network")},
        { new NotificationModule(this), tr("Notification")},
        { new SoundModule(this), tr("Sound")},
        { new BluetoothModule(this), tr("Bluetooth")},
        { new DatetimeModule(this), tr("Date and Time")},
        { new PowerModule(this), tr("Power")},
        { new MouseModule(this), tr("Mouse")},
        { new WacomModule(this), tr("Drawing Tablet")},
        { new KeyboardModule(this), tr("Keyboard and Language")},
        { new UpdateModule(this), tr("Updates")},
        { new SystemInfoModule(this), tr("System Info")},
        { new CommonInfoModule(this), tr("General Settings")},
    };

    //读取加载一级菜单的插件
    if (InsertPlugin::instance(this, this)->needPushPlugin("mainwindow"))
        InsertPlugin::instance()->pushPlugin(m_modules);

    //通过gsetting设置某模块是否显示,默认都显示
    m_moduleSettings = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);

    connect(m_moduleSettings, &QGSettings::changed, this, [ & ](const QString & keyName) {
        if (keyName != "hideModule" && keyName != GSETTINGS_HIDE_MODULE) {
            return;
        }
        updateModuleVisible();
    });
    updateModuleVisible();

    //通过gsetting获取版本类型，设置某模块是否显示
    if (QGSettings::isSchemaInstalled("com.deepin.dde.control-versiontype")) {
        m_versionType  = new QGSettings("com.deepin.dde.control-versiontype", QByteArray(), this);
        auto versionTypeList =  m_versionType->get(GSETTINGS_HIDE_VERSIONTYPR).toStringList();
        for (auto i : m_modules) {
            if (versionTypeList.contains((i.first->name()))) {
                setModuleVisible(i.first, false);
            }
        }
    }

    bool isIcon = m_contentStack.empty();

    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(it->first->icon());
        item->setText(it->second);
        if (it->first->name() == "systeminfo" && DSysInfo::DeepinDesktop == DSysInfo::deepinType())
            item->setIcon(QIcon::fromTheme(QString("dcc_nav_deepin_systeminfo")));
        if (it->first->name() == "commoninfo") {
            item->setAccessibleText("SECOND_MENU_COMMON");
        } else {
            item->setAccessibleText(it->second);
        }

        //目前只有"update"模块需要使用右上角的角标，其他模块还是使用旧的位置数据设置
        //若其他地方需要使用右上角的角标，可在下面if处使用“||”添加对应模块的name()值
        if (it->first->name() == "update" && m_updateVisibale) {
            auto action1 = new DViewItemAction(Qt::AlignTop | Qt::AlignRight, QSize(ActionIconSize, ActionIconSize), QSize(ActionIconSize, ActionIconSize), false);
            action1->setIcon(QIcon(":/icons/deepin/builtin/icons/dcc_common_subscript.svg"));
            action1->setVisible(false);
            auto action2 = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter, QSize(ActionListSize, ActionListSize), QSize(ActionListSize, ActionListSize), false);
            action2->setIcon(QIcon(":/icons/deepin/builtin/icons/dcc_common_subscript.svg"));
            action2->setVisible(false);
            item->setActionList(Qt::Edge::RightEdge, {action1, action2});
            CornerItemGroup group;
            group.m_name = it->first->name();
            group.m_action.first = action1;
            group.m_action.second = action2;
            group.m_index = m_navModel->rowCount();
            m_remindeSubscriptList.append(group);
            if (action2->isVisible())
                item->setData(QVariant::fromValue(QMargins(ActionIconSize + 15, 0, 0, 0)), Dtk::MarginsRole);
        } else {
            item->setData(NavItemMargin, Dtk::MarginsRole);
        }

        m_navModel->appendRow(item);
        m_searchWidget->addModulesName(it->first->name(), it->second, it->first->icon(), it->first->translationPath());
    }

    resetNavList(isIcon);

    modulePreInitialize(m);
    //设置 触控板，指点杆 是否存在
    m_searchWidget->setRemoveableDeviceStatus(tr("Touchpad"), getRemoveableDeviceStatus(tr("Touchpad")));
    m_searchWidget->setRemoveableDeviceStatus(tr("TrackPoint"), getRemoveableDeviceStatus(tr("TrackPoint")));

    QElapsedTimer et;
    et.start();
    //after initAllModule to load ts data
    m_searchWidget->setLanguage(QLocale::system().name());
    qDebug() << QString("load search info with %1ms").arg(et.elapsed());
}

void MainWindow::updateWinsize()
{
    int w = QGuiApplication::primaryScreen()->geometry().width();
    int h = QGuiApplication::primaryScreen()->geometry().height();
    WidgetMinimumWidth = qMin(w, 820);
    WidgetMinimumHeight = qMin(h, 634);
    setMinimumSize(QSize(WidgetMinimumWidth, WidgetMinimumHeight));
}

void MainWindow::setSpecialThreeMenuVisible(QString name, bool flag)
{
    m_searchWidget->addSpecialThreeMenuMap(name, flag);
}

void MainWindow::updateModuleVisible()
{
    m_hideModuleNames = m_moduleSettings->get(GSETTINGS_HIDE_MODULE).toStringList();
    for (auto i : m_modules) {
        if (m_hideModuleNames.contains((i.first->name()))) {
            setModuleVisible(i.first, false);
        } else if (!i.first->deviceUnavailabel()) {
            setModuleVisible(i.first, true);
        }
    }
}

void MainWindow::modulePreInitialize(const QString &m)
{
    for (auto it = m_modules.cbegin(); it != m_modules.cend(); ++it) {
        QElapsedTimer et;
        et.start();
        it->first->preInitialize(m == it->first->name());
        qDebug() << QString("initialize %1 module using time: %2ms")
                 .arg(it->first->name())
                 .arg(et.elapsed());

        setModuleVisible(it->first, it->first->isAvailable());
    }
}

void MainWindow::popWidget()
{
    if (m_topWidget) {
        if(0 < m_contentStack.count())
            m_lastPushWidget = m_contentStack.last().second;
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
        m_lastThirdPage.first = nullptr;
        m_lastThirdPage.second = nullptr;
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
    resetNavList(m_contentStack.isEmpty());
}

void MainWindow::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_UNUSED(animation)

    if (module == m_moduleName && page.isEmpty()) { //当前模块且未指定页面，直接返回
        activateWindow();
        return;
    }

    // FIXME: 通过dbus触发切换page菜单界面时，应先检测当前page界面是否存在模态对话框在上层显示，
    // 若存在则先将其所有对话框强制关闭，再执行切换page界面显示
    QWidget *lastPushWidget = qobject_cast<QWidget*>(m_lastPushWidget);//判断指针是否失效
    if (lastPushWidget && lastPushWidget->isVisible()) {
        QList<QDialog *> dialogList = lastPushWidget->findChildren<QDialog *>();
        for (QDialog *dlg : dialogList) {
            if (dlg && dlg->isVisible()) {
                dlg->done(QDialog::Rejected);
            }
        }
    }

//    qDebug() << Q_FUNC_INFO;
    if (!isModuleAvailable(module) && !module.isEmpty()) {
        qDebug() << QString("get error module name %1!").arg(module);
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::InvalidArgs,
                           "cannot find module that name is " + module);

            if (!isVisible()) {
                close();
            }
        }

        return;
    }

    auto findModule = [this](const QString & str)->ModuleInterface * {
        auto res = std::find_if(m_modules.begin(), m_modules.end(), [ = ](const QPair<ModuleInterface *, QString> &data)->bool{
            return data.first->name() == str;
        });

        if (res != m_modules.end()) {
            return (*res).first;
        }

        return nullptr;
    };

    auto pm = findModule(module);
    Q_ASSERT(pm);

    qDebug() << page;
    QStringList pages = page.split(",");
    if (pages[0] != "" && !pm->availPage().contains(pages[0])) {
        qDebug() << QString("get error page path %1!").arg(pages[0]);
        if (calledFromDBus()) {
            auto err = QString("cannot find page path that name is %1 on module %2.")
                       .arg(pages[0]).arg(module);
            sendErrorReply(QDBusError::InvalidArgs, err);

            if (!isVisible()) {
                close();
            }
        }

        return;
    }

    raise();

    onEnterSearchWidget(module, page);
    // Note: 当直接进入模块界面(二级界面)，先将模块界面显示出来，在加载首界面
    QTimer::singleShot(0, this, [ = ] {
        if (isMinimized() || !isVisible())
            showNormal();

        activateWindow();
    });
}

void MainWindow::setModuleSubscriptVisible(const QString &module, bool bIsDisplay)
{
    QPair<DViewItemAction *, DViewItemAction *> m_pair(nullptr, nullptr);
    int index = 0;
    for (const auto &k : m_remindeSubscriptList) {
        if (module == k.m_name) {
            m_pair = k.m_action;
            index = k.m_index;
        }
    }

    if (m_pair.first == nullptr) {
        return;
    }

    if (m_navView->viewMode() == QListView::IconMode) {
        if (m_pair.first->isVisible() != bIsDisplay) {
            m_pair.first->setVisible(bIsDisplay);
            if (bIsDisplay) {
                m_navModel->item(index, 0)->setData(QVariant::fromValue(QMargins(ActionIconSize + 15, 0, 0, 0)), Dtk::MarginsRole);
            } else {
                m_navModel->item(index, 0)->setData(QVariant::fromValue(QMargins(0, 0, 0, 0)), Dtk::MarginsRole);
            }
        }
    } else {
        if (m_pair.second->isVisible() != bIsDisplay) {
            m_pair.second->setVisible(bIsDisplay);
        }
    }
    m_navView->update();
}

bool MainWindow::isModuleAvailable(const QString &m)
{
    auto res = std::find_if(m_modules.begin(), m_modules.end(), [ = ](const QPair<ModuleInterface *, QString> &data)->bool{
        return data.first->name() == m;
    });

    if (res != m_modules.end()) {
        return (*res).first->isAvailable();
    }

    qDebug() << QString("can not fine module named %1!").arg(m);
    return false;
}

void MainWindow::toggle()
{
    raise();
    if (isMinimized() || !isVisible())
        showNormal();

    activateWindow();
}

void MainWindow::openManual()
{
    QString helpTitle = m_moduleName;
    if (helpTitle.isEmpty()) {
        helpTitle = "controlcenter";
    }
    const QString dmanInterface = "com.deepin.Manual.Open";
    QDBusInterface interface(dmanInterface,
                             "/com/deepin/Manual/Open",
                             dmanInterface,
                             QDBusConnection::sessionBus());

    QDBusMessage reply = interface.call("OpenTitle", "dde", helpTitle);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "reply.type() = " << reply.type();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        if (m_topWidget) {
            popWidget();
        }
        break;
    case Qt::Key_F1: {
        openManual();
        break;
    }
    default:
        break;
    }

    DMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && m_navView && m_navView->viewMode() == QListView::ListMode) {
        if (QEvent::WindowDeactivate == event->type() || QEvent::WindowActivate == event->type()) {
            DPalette pa = DApplicationHelper::instance()->palette(m_navView);
            QColor base_color = palette().base().color();

            if (QEvent::WindowDeactivate == event->type()) {
                base_color = DGuiApplicationHelper::adjustColor(base_color, 0, 0, 0, 0, 0, 0, -5);
                pa.setColor(DPalette::ItemBackground, base_color);
            } else if (QEvent::WindowActivate == event->type()) {
                pa.setColor(DPalette::ItemBackground, base_color);
            }

            DApplicationHelper::instance()->setPalette(m_navView, pa);
        }
    }

    if (event->type() == QEvent::Shortcut) {
        auto ke = static_cast<QShortcutEvent *>(event);
        if (ke->key() == Qt::Key_F1) {
            openManual();
            return true;
        }
    }

    return  DMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (m_needRememberLastSize && event->oldSize() != event->size()) {
        m_lastSize = event->oldSize();
    }
    m_needRememberLastSize = true;

    DMainWindow::resizeEvent(event);

    auto dstWidth = event->size().width();
    if (four_widget_min_widget > dstWidth) {
        //update只有当Mirror页面存在时，才会有第二级页面；size变化小了，需要恢复成topWidget
        if ((m_contentStack.size() == 3) || (m_contentStack.size() == 2 && m_contentStack.first().first->name() == "update")) {
            auto ite = m_contentStack.pop();
            pushTopWidget(ite.first, ite.second);
        } else {
            qDebug() << "Not satisfied , can't back.";
        }
    } else if (four_widget_min_widget <= dstWidth) {
        if (!m_topWidget) {
            qDebug() << " The top widget is nullptr.";
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
        m_topWidget->curWidget()->setMinimumWidth(dstWidth / 2 - 40);
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
        m_navView->setIconSize(ListViweItemIconSize);
        m_navView->setItemSize(ListViweItemSize);
        m_navView->setItemSpacing(0);
        m_navView->setSpacing(20);
        m_navView->clearSelection();
        m_navView->setSelectionMode(QAbstractItemView::NoSelection);

        //Icon模式，"update"使用右上角角标Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if (m_modules.at(i).first->name() == data.m_name) {
                    data.m_action.first->setVisible(data.m_action.second->isVisible());
                    data.m_action.second->setVisible(false);
                    if (data.m_action.first->isVisible())
                        m_navModel->item(i, 0)->setData(QVariant::fromValue(QMargins(ActionIconSize + 15, 0, 0, 0)), Dtk::MarginsRole);
                    else
                        m_navModel->item(i, 0)->setData(QVariant::fromValue(QMargins()), Dtk::MarginsRole);
                    break;
                }
            }
        }

        DStyle::setFrameRadius(m_navView, 18);
        m_rightView->hide();
        m_backwardBtn->setEnabled(false);
    } else {
        //The second page will Covered with fill blank areas
        m_navView->setViewportMargins(QMargins(10, 10, 10, 10));
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setMinimumWidth(first_widget_min_width);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_navView->setIconSize(ListViweItemIconSize_ListMode);
        m_navView->setItemSize(ListViweItemSize_ListMode);
        m_navView->setSpacing(0);
        m_navView->setSelectionMode(QAbstractItemView::SingleSelection);

        //List模式，"update"使用统一Margin
        for (auto data : m_remindeSubscriptList) {
            for (int i = 0; i < m_navModel->rowCount(); i++) {
                if (m_modules.at(i).first->name() == data.m_name) {
                    m_navModel->item(i, 0)->setData(NavItemMargin, Dtk::MarginsRole);
                    data.m_action.second->setVisible(data.m_action.first->isVisible());
                    data.m_action.first->setVisible(false);
                    break;
                }
            }
        }

        DStyle::setFrameRadius(m_navView, 8);
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
    QStringList widgetPages = widget.split(",");
    QString widgetFirst = widgetPages[0];
    qDebug() << Q_FUNC_INFO << " moduleName : " << moduleName << " , widget :" << widgetFirst;

    for (int firstCount = 0; firstCount < m_modules.count(); firstCount++) {
        //Compare moduleName and m_modules.second(module name)
        if (moduleName == m_modules[firstCount].first->name()) {
            //enter first level widget
            m_navView->setCurrentIndex(m_navView->model()->index(firstCount, 0));

            if (m_topWidget) {
                popAllWidgets();
            }
            onFirstItemClick(m_navView->model()->index(firstCount, 0));

            //当从dbus搜索进入这里时，如果传入的page参数错误，则会使用m_widgetName保存一个错的数据。
            //这样当前界面实际有进入的模块的界面界面。这样会出现重复调用该函数显示同一个界面的情况
            //目前看对程序没有影响
            m_firstCount = firstCount;
            m_widgetName = widgetFirst;

            //notify related module load widget
//            QTimer::singleShot(0, this, [ = ] { //avoid default and load sequence in time
            auto errCode = m_modules[m_firstCount].first->load(widget);
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
    bool bFinalVisible = visible;
    if (bFinalVisible && m_hideModuleNames.contains(inter->name())) {
        bFinalVisible = false;
    }
    inter->setAvailable(bFinalVisible);

    auto find_it = std::find_if(m_modules.cbegin(),
                                m_modules.cend(),
    [ = ](const QPair<ModuleInterface *, QString> &pair) {
        return pair.first == inter;
    });

    if (find_it != m_modules.cend()) {
        m_navView->setRowHidden(find_it - m_modules.cbegin(), !bFinalVisible);
        Q_EMIT moduleVisibleChanged(find_it->first->name(), bFinalVisible);

        qDebug() << "[SearchWidget] find_it->first->name() : " << find_it->first->name() << bFinalVisible;
        if ("bluetooth" == find_it->first->name()) {
            if (bFinalVisible) {
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
            if (bFinalVisible) {
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
            if (bFinalVisible) {
                m_searchWidget->removeUnExsitData(tr("Cloud Sync"));
            } else {
                m_searchWidget->addUnExsitData(tr("Cloud Sync"));
            }
        } else if ("commoninfo" == find_it->first->name()) {
            if (bFinalVisible) {
                m_searchWidget->removeUnExsitData(tr("General Settings"));
            } else {
                m_searchWidget->addUnExsitData(tr("General Settings"));
            }
        } else if ("update" == find_it->first->name()) {
            m_updateVisibale = bFinalVisible;
            if (bFinalVisible) {
                m_searchWidget->removeUnExsitData(tr("Updates"));
            } else {
                m_searchWidget->addUnExsitData(tr("Updates"));
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Not found module!";
    }
}

void MainWindow::pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type)
{
    if (!inter)  {
        qDebug() << Q_FUNC_INFO << " inter is nullptr";
        return;
    }

    if (!w)  {
        qDebug() << Q_FUNC_INFO << " widget is nullptr";
        return;
    }

    m_lastPushWidget = w;
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

    resetTabOrder();
}

//First save the third level page, Then pop the third level page
//Next set the new page as the third level page,
//Finally when in the new third level page clicked "pop" button , return to the old three level page
void MainWindow::replaceThirdWidget(ModuleInterface *const inter, QWidget *const w)
{
    if (m_contentStack.count() != 2)
        return;

    if (m_lastThirdPage.second) {
        m_lastThirdPage.first = nullptr;
        m_lastThirdPage.second = nullptr;
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

    connect(m_topWidget, &FourthColWidget::signalBack, this, &MainWindow::onBack);

    m_topWidget->setFixedHeight(height() - this->titlebar()->height());
    m_topWidget->move(0, titlebar()->height());

    resetNavList(m_contentStack.empty());
}

//为后面平铺模式留接口
void MainWindow::pushFinalWidget(ModuleInterface *const inter, QWidget *const w)
{
    w->layout()->setMargin(0);
    w->setContentsMargins(0, 8, 0, 8);
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

        //解决插件二级菜单右侧两角为圆角的问题[正常情况圆角应该在三级菜单的右侧两角和二级菜单的左侧两角]
        m_contentStack.at(1).second->show();
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
        qDebug() << Q_FUNC_INFO << " error widget content conut : " << contentCount;
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
    DPalette pa = DApplicationHelper::instance()->palette(m_navView);
    QColor base_color = palette().base().color();

    if (m_navView->viewMode() == QListView::IconMode) {
        DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(base_color);

        if (ct == DGuiApplicationHelper::LightType) {
            pa.setBrush(DPalette::ItemBackground, palette().base());
        } else {
            base_color = DGuiApplicationHelper::adjustColor(base_color, 0, 0, +5, 0, 0, 0, 0);
            pa.setColor(DPalette::ItemBackground, base_color);
        }

    } else {
        pa.setColor(DPalette::ItemBackground, base_color);
    }

    DApplicationHelper::instance()->setPalette(m_navView, pa);
}

void MainWindow::onFirstItemClick(const QModelIndex &index)
{
    ModuleInterface *inter = m_modules[index.row()].first;

    if (!m_contentStack.isEmpty() && m_contentStack.last().first == inter) {
        return;
    }

    m_navView->setFocus();
    popAllWidgets();

    if (!m_initList.contains(inter)) {
        inter->initialize();
        m_initList << inter;
    }
    m_moduleName = inter->name();
    setCurrModule(inter);
    inter->active();
    m_navView->resetStatus(index);
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
    showWidget->setMinimumWidth(this->parentWidget()->width() / 2 - 40);
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

void FourthColWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_curWidget->geometry().contains(event->pos()))
        return;
    Q_EMIT signalBack();
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

void MainWindow::setSearchPath(ModuleInterface *const inter) const
{
    m_searchWidget->addModulesName(inter->name(), inter->displayName(), inter->icon(), inter->translationPath());
}
