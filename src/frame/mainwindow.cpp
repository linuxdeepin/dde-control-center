/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "interface/moduleobject.h"
#include "pluginmanager.h"
#include "searchwidget.h"
#include "listview.h"
#include "listitemdelegate.h"
#include "tabview.h"
#include "tabitemdelegate.h"
#include "moduledatamodel.h"
#include "widgets/utils.h"

#include <DBackgroundGroup>
#include <DIconButton>
#include <DTitlebar>
#include <DConfig>
#include <DWidgetUtil>
#include <DButtonBox>
#include <DPushButton>
#include <DListView>
#include <DPaletteHelper>
#include <DGuiApplicationHelper>
#include <DPalette>
#include <DFrame>

#include <QMenu>
#include <QLayout>
#include <QStandardItemModel>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QScrollArea>
#include <QLabel>
#include <QScrollBar>
#include <QShortcut>
#include <QTimer>
#include <QColor>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

const QSize MainWindowMininumSize(QSize(800, 600));

const QString ControlCenterConfig = QStringLiteral("dde-control-center");
const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");

const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 188;

const QSize ListViweItemIconSize_IconMode(84, 84);
const QSize ListViweItemSize_IconMode(280, 84);
const QSize ListViweItemIconSize_ListMode(32, 32);
const QSize ListViweItemSize_ListMode(168, 48);


MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
    , m_contentWidget(new QWidget(this))
    , m_backwardBtn(new DIconButton(QStyle::SP_ArrowBack, this))
    , m_dconfig(new DConfig(ControlCenterConfig, QString(), this))
    , m_searchWidget(new SearchWidget(this))
    , m_rootModule(nullptr)
    , m_currentModule(nullptr)
    , m_pluginManager(nullptr)
    , m_mainView(nullptr)
{
    initUI();
    initConfig();
    loadModules();

    connect(m_searchWidget, &SearchWidget::notifySearchUrl, this, [this] (const QString &url) {
        showPage(url, UrlType::DisplayName);
    });

}

MainWindow::~MainWindow()
{
    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, width());
        m_dconfig->setValue(HeightConfig, height());
    }
    if (m_pluginManager) {
        m_pluginManager->deleteLater();
    }
}

int MainWindow::getScrollPos(const int index)
{
    int pos = 0;
    for (int i = 0; i < qMin<int>(m_pages.count(), index); i++) {
        pos += m_pages[i]->height();
    }
    return pos;
}

void MainWindow::showPage(const QString &url, const UrlType &uType)
{
    qInfo() << "show page url:" << url;
    if (url.isEmpty() || url == "/") {
        toHome();
    }
    if (!m_rootModule) {
        QTimer::singleShot(10, this, [ = ] {
            showPage(url, uType);
        });
        return;
    }
    showPage(m_rootModule, url, uType);
}

void MainWindow::showPage(ModuleObject *const module, const QString &url, const UrlType &uType)
{
    QStringList names = url.split('/');
    const QString &name = names.takeFirst();
    int index = -1;
    QString childName;
    for (auto child : module->childrens()) {
        if (uType == UrlType::Name)
            childName = child->name();
        if (uType == UrlType::DisplayName)
            childName = child->moduleData()->DisplayName;
        if (childName == name || child->moduleData()->ContentText.contains(name)) {
            index = module->childrens().indexOf(child);
            Q_EMIT module->activeChild(index);
            return showPage(child, names.join('/'), uType);
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateMainView();
    }
    return DMainWindow::changeEvent(event);
}

void MainWindow::initUI()
{
    setMinimumSize(MainWindowMininumSize);
    m_contentWidget->setAccessibleName("contentwindow");
    m_contentWidget->setObjectName("contentwindow");
    setCentralWidget(m_contentWidget);

    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->setContentsMargins(ZeroMargins);


    auto menu = titlebar()->menu();
    if (!menu) {
        qDebug() << "menu is nullptr, create menu!";
        menu = new QMenu;
    }
    menu->setAccessibleName("titlebarmenu");
    titlebar()->setMenu(menu);

    auto action = new QAction(tr("Help"), menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this] {
        openManual();
    });

    m_backwardBtn->setAccessibleName("backwardbtn");

    titlebar()->addWidget(m_backwardBtn, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar()->setIcon(QIcon::fromTheme("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, &MainWindow::toHome);

    m_searchWidget->setMinimumSize(300, 36);
    m_searchWidget->setAccessibleName("SearchModule");
    m_searchWidget->lineEdit()->setAccessibleName("SearchModuleLineEdit");
    titlebar()->addWidget(m_searchWidget, Qt::AlignCenter);
}

void MainWindow::initConfig()
{
    if (!m_dconfig->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_dconfig->name(), m_dconfig->subpath());
        return;
    }

    auto w = m_dconfig->value(WidthConfig).toInt();
    auto h = m_dconfig->value(HeightConfig).toInt();
    resize(w, h);
    Dtk::Widget::moveToCenter(this);
}

void MainWindow::loadModules()
{
    QFutureWatcher<void> *watcher= new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this] {
        showModule(m_rootModule, m_contentWidget);
        m_searchWidget->setModuleObject(m_rootModule);
    });

    QFuture<void> future = QtConcurrent::run([this] {
        if (m_pluginManager)
            m_pluginManager->deleteLater();
        m_pluginManager = new PluginManager;
        m_pluginManager->loadModules();
        m_rootModule = m_pluginManager->rootModule();
        m_pluginManager->moveToThread(qApp->thread());
    });
    watcher->setFuture(future);
}

void MainWindow::toHome()
{
    m_rootModule->setChildType(ModuleObject::ChildType::MainIcon);
    setCurrentModule(nullptr);
    showModule(m_rootModule, m_contentWidget);
}

void MainWindow::updateMainView()
{
    if (!m_mainView)
        return;
    // set background
    DPalette pa = DPaletteHelper::instance()->palette(m_mainView);
    QColor baseColor = palette().base().color();
    DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(baseColor);
    if (ct == DGuiApplicationHelper::LightType) {
        pa.setBrush(DPalette::ItemBackground, palette().base());
    }
    else {
        baseColor = DGuiApplicationHelper::adjustColor(baseColor, 0, 0, +5, 0, 0, 0, 0);
        pa.setColor(DPalette::ItemBackground, baseColor);
    }
    DPaletteHelper::instance()->setPalette(m_mainView, pa);
}

void MainWindow::clearPage(QWidget *const widget)
{
    QLayout *layout = widget->layout();
    QScrollArea *area = qobject_cast<QScrollArea *>(widget);
    if (area)
        area->widget()->deleteLater();
    if (layout) {
        while (QLayoutItem *child = layout->takeAt(0))
        {
            layout->removeWidget(child->widget());
            child->widget()->deleteLater();
            layout->removeItem(child);
            delete child;
        }
        delete layout;
    }
}

void MainWindow::showModule(ModuleObject *const module, QWidget *const parent, const int index)
{
    if (!module || !parent)
        return;
    if (module->childrens().isEmpty())
        return;
    if (module->findChild(currentModule()) >= 0)
        return;

    qDebug() << QString("module name:%1, index:%2, children size:%3").arg(module->name()).arg(index).arg(module->childrens().size());
    module->active();
    clearPage(parent);

    switch (module->childType())
    {
    case ModuleObject::ChildType::MainIcon:
        showModuleMainIcon(module, parent, index);
        break;
    case ModuleObject::ChildType::MainList:
        // 展开主菜单时，原来的主菜单被析构，需清空其指针
        m_mainView = nullptr;
        setCurrentModule(module);
        showModuleMainList(module, parent, index);
        break;
    case ModuleObject::ChildType::HList:
        setCurrentModule(module);
        showModuleHList(module, parent, index);
        break;
    case ModuleObject::ChildType::VList:
        setCurrentModule(module);
        showModuleVList(module, parent, index);
        break;
    case ModuleObject::ChildType::Page:
        setCurrentModule(module);
        showModulePage(module, parent, index);
    default:
        break;
    }
}

void MainWindow::showModuleMainIcon(ModuleObject *const module, QWidget *const parent, const int index)
{
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    parent->setLayout(vlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);

    ListView *view = new ListView(parent);
    ListItemDelegate *delegate = new ListItemDelegate(view);
    view->setItemDelegate(delegate);
    vlayout->addWidget(view);
    view->setModel(model);
    view->setFrameShape(QFrame::Shape::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setIconSize(ListViweItemIconSize_IconMode);
    view->setGridSize(ListViweItemSize_IconMode);
    view->setSpacing(20);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setViewMode(ListView::IconMode);
    view->setAcceptDrops(false);
    view->setAlignment(Qt::AlignCenter);
    m_mainView = view;
    updateMainView();

    m_backwardBtn->setEnabled(false);

    auto onClicked = [this, module, parent] (const QModelIndex &index) {
        module->setChildType(ModuleObject::ChildType::MainList);
        m_backwardBtn->setEnabled(true);
        showModule(module, parent, index.row());
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, this , [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);
    if (index < 0)
        return;
    onClicked(model->index(index, 0));
}

void MainWindow::showModuleMainList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    parent->setLayout(hlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);
    QScrollArea *area = new QScrollArea(parent);
    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidgetResizable(true);

    ListView *view = new ListView(parent);
    ListItemDelegate *delegate = new ListItemDelegate(view);
    view->setItemDelegate(delegate);
    hlayout->addWidget(view, 1);
    hlayout->addWidget(area, 5);
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    view->setModel(model);
    view->setFrameShape(QFrame::Shape::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    view->setIconSize(ListViweItemIconSize_ListMode);
    view->setGridSize(ListViweItemSize_ListMode);
    view->setSpacing(1);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto onClicked = [this, module, area, view] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);
        showModule(module->childrens()[row], area, 0);
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(module, &ModuleObject::removedChild, area, [this](ModuleObject *const childModule) {
        if (childModule->findChild(currentModule()) >= 0) {
            toHome();
        }
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);

    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void MainWindow::showModuleHList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    parent->setLayout(vlayout);

    DFrame *dframeTab = new DFrame(parent);
    QHBoxLayout *hlayout = new QHBoxLayout(dframeTab);
    hlayout->setMargin(3);
    hlayout->setSpacing(0);
    TabView *view = new TabView(parent);
    hlayout->addWidget(view);
    ModuleDataModel *model = new ModuleDataModel(view);
    TabItemDelegate *delegate = new TabItemDelegate(view);
    view->setModel(model);
    view->setItemDelegate(delegate);
    vlayout->addWidget(dframeTab, 1, Qt::AlignCenter);

    QScrollArea *area = new QScrollArea(parent);
    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidgetResizable(true);
    area->setContentsMargins(ZeroMargins);

    model->setData(module);

    DFrame *dframe = new DFrame(parent);

    connect(module, &ModuleObject::removedChild, this, [this] (ModuleObject *const module) {
        if (module->findChild(currentModule()) >= 0) {
            toHome();
        }
    });

    auto onClicked = [ = ] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);

        // 判断子项是否为垂直菜单，如果是则需要加上Frame
        if (vlayout->count() >= 2)
            vlayout->takeAt(vlayout->count() - 1);
        if (module->childrens()[row]->childType() == ModuleObject::ChildType::VList) {
            vlayout->addWidget(dframe, 6);
            dframe->show();
            area->hide();
            showModule(module->childrens()[row], dframe, 0);
        } else {
            dframe->hide();
            area->show();
            vlayout->addWidget(area, 6);
            showModule(module->childrens()[row], area, 0);
        }
    };

    connect(view, &TabView::activated, view, &TabView::clicked);
    connect(view, &TabView::clicked, this, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);
    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void MainWindow::showModuleVList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    parent->setLayout(hlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);
    QScrollArea *area = new QScrollArea(parent);
    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidgetResizable(true);

    DListView *view = new DListView(parent);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *vlayout = new QVBoxLayout;
    widget->setLayout(vlayout);
    vlayout->addWidget(view);
    if (module->extraButton())
        vlayout->addWidget(getExtraPage(module->extraButton()));
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(widget, 1);
    hlayout->addWidget(new DVerticalLine);
    hlayout->addWidget(area, 5);

    view->setModel(model);
    view->setFrameShape(QFrame::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    view->setIconSize(ListViweItemIconSize_ListMode);
    view->setItemSize(ListViweItemSize_ListMode);
    view->setSpacing(0);
    view->setItemSpacing(1);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto onClicked = [this, module, area, view] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);
        showModule(module->childrens()[row], area, 0);
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(module, &ModuleObject::extraButtonClicked, area, [this, area, module] {
        clearPage(area);
        setCurrentModule(nullptr);
        area->setWidget(module->page());
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);

    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void MainWindow::showModulePage(ModuleObject *const module, QWidget *const parent, const int index)
{
    QScrollArea *area = qobject_cast<QScrollArea *>(parent);

    QWidget *areaWidget = new QWidget(area);
    area->setWidget(areaWidget);
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    areaWidget->setLayout(vlayout);

    m_pages.clear();
    for (auto child : module->childrens()) {
        auto page = getPage(child->page(), child->moduleData()->DisplayName);
        if (page) {
            m_pages << page;
            vlayout->addWidget(page);
        }
        child->active();

        connect(child, &ModuleObject::moduleDataChanged, area, [ = ] {
            vlayout->removeWidget(page);
            const int index = m_pages.indexOf(page);
            m_pages.removeOne(page);
            page->deleteLater();
            auto newPage = getPage(child->page(), child->moduleData()->DisplayName);
            if (newPage) {
                m_pages.insert(index, newPage);
                vlayout->insertWidget(index, newPage);
            }
        });
    }
    if (m_pages.count() > 1)
        vlayout->addStretch(1);

    if (module->extraButton())
        vlayout->addWidget(getExtraPage(module->extraButton()), 0, Qt::AlignBottom);
    area->verticalScrollBar()->setSliderPosition(getScrollPos(index));

    connect(module, &ModuleObject::activeChild, area, [this, area] (const int index) {
        area->verticalScrollBar()->setSliderPosition(getScrollPos(index));
    });

    connect(module, &ModuleObject::removedChild, area, [this, module, vlayout](ModuleObject *const childModule) {
        int index = module->childrens().indexOf(childModule);
        QWidget *w = m_pages.at(index);
        vlayout->removeWidget(w);
        w->deleteLater();
        m_pages.removeAt(index);
    });
    auto addChild = [this, module, vlayout](ModuleObject *const childModule) {
        int index = module->childrens().indexOf(childModule);
        auto newPage = getPage(childModule->page(), childModule->moduleData()->DisplayName);
        if (newPage) {
            m_pages.insert(index, newPage);
            vlayout->insertWidget(index, newPage);
        }
    };
    connect(module, &ModuleObject::insertedChild, area, addChild);
    connect(module, &ModuleObject::appendedChild, area, addChild);

    connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto child : module->childrens()) {
            child->deactive();
        }
        module->deactive();
    });
}

QWidget* MainWindow::getPage(QWidget *const widget, const QString &title)
{
    if (!widget)
        return nullptr;
    QLabel *titleLbl = new QLabel(title, this);
    QWidget *page = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(page);
    page->setLayout(vLayout);
    vLayout->addWidget(titleLbl, 0, Qt::AlignTop);
    vLayout->addWidget(widget, 1, Qt::AlignTop);
    if (title.isEmpty()) {
        titleLbl->setVisible(false);
    }
    return page;
}

QWidget* MainWindow::getExtraPage(QWidget *const widget)
{
    QWidget *tmpWidget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(tmpWidget);
    tmpWidget->setLayout(vLayout);
    vLayout->addWidget(widget);
    vLayout->setSpacing(0);
    return tmpWidget;
}

void MainWindow::openManual()
{
    QString helpTitle;
    if (currentModule())
        helpTitle = currentModule()->name();
    if (helpTitle.isEmpty())
        helpTitle = "controlcenter";

    const QString &dmanInterface = "com.deepin.Manual.Open";
    QDBusInterface interface(dmanInterface,
                             "/com/deepin/Manual/Open",
                             dmanInterface,
                             QDBusConnection::sessionBus());

    QDBusMessage reply = interface.call("OpenTitle", "dde", helpTitle);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Open manual failed, error message:" << reply.errorMessage();
    }
}
