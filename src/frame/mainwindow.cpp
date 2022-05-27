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
#include <QFuture>
#include <QFutureWatcher>
#include <QScrollArea>
#include <QLabel>
#include <QScrollBar>
#include <QShortcut>
#include <QTimer>
#include <QColor>

#include "layout/layoutmanager.h"

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

#define DCC_CONFIG_HIDDEN 0x20000000
#define DCC_CONFIG_DISABLED 0x10000000
#define setConfigHidden(hide) setFlagState(DCC_CONFIG_HIDDEN, hide)
#define setConfigDisabled(disabled) setFlagState(DCC_CONFIG_DISABLED, disabled)

const QSize MainWindowMininumSize(QSize(800, 600));

const QString ControlCenterConfig = QStringLiteral("org.deepin.dde.control-center");
const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");
const QString HideConfig = QStringLiteral("hideModule");
const QString DisableConfig = QStringLiteral("disableModule");

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
    , m_contentWidget(new QWidget(this))
    , m_backwardBtn(new DIconButton(QStyle::SP_ArrowBack, this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", ControlCenterConfig, QString(), this))
    , m_searchWidget(new SearchWidget(this))
    , m_rootModule(new ModuleObject(this))
    , m_layoutManager(new LayoutManager())
    , m_pluginManager(new PluginManager(this))
{
    qRegisterMetaType<LayoutBase *>("LayoutBase *");
    qRegisterMetaType<ModuleObject *>("ModuleObject *");

    initUI();
    initConfig();
    loadModules();

    connect(m_searchWidget, &SearchWidget::notifySearchUrl, this, [this](const QString &url) {
        showPage(url, UrlType::DisplayName);
    });
}

MainWindow::~MainWindow()
{
    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, width());
        m_dconfig->setValue(HeightConfig, height());
    }
}

void MainWindow::showPage(const QString &url, const UrlType &uType)
{
    qInfo() << "show page url:" << url;
    if (url.isEmpty() || url == "/") {
        toHome();
    }
    if (!m_rootModule) {
        QTimer::singleShot(10, this, [=] {
            showPage(url, uType);
        });
        return;
    }
    showPage(m_rootModule, url, uType);
}

void MainWindow::showPage(ModuleObject *const module, const QString &url, const UrlType &uType)
{
    ModuleObject *obj = module;
    QStringList names = url.split('/');
    while (!names.isEmpty() && obj) {
        const QString &name = names.takeFirst();
        QString childName;
        for (auto child : obj->childrens()) {
            if (uType == UrlType::Name)
                childName = child->name();
            if (uType == UrlType::DisplayName)
                childName = child->displayName();
            if (childName == name || child->contentText().contains(name)) {
                obj = child;
            }
        }
    }

    showModule(obj, m_contentWidget);
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

    updateModuleConfig(HideConfig);
    updateModuleConfig(DisableConfig);
    connect(m_dconfig, &DConfig::valueChanged, this, &MainWindow::updateModuleConfig);
}

void MainWindow::configModule(QString url, ModuleObject *module)
{
    module->setFlagState(DCC_CONFIG_HIDDEN, m_hideModule.contains(url));
    module->setFlagState(DCC_CONFIG_DISABLED, m_disableModule.contains(url));
}

QSet<QString> findAddItems(QSet<QString> *oldSet, QSet<QString> *newSet)
{
    QSet<QString> addSet;
    for (auto &&key : *newSet) {
        if (!oldSet->contains(key)) {
            addSet.insert(key);
        }
    }
    return addSet;
}

void MainWindow::updateModuleConfig(const QString &key)
{
    QSet<QString> oldModuleConfig;
    QSet<QString> *newModuleConfig = nullptr;
    DCC_LAYOUT_TYPE type = DCC_CONFIG_HIDDEN;
    if (key == HideConfig) {
        type = DCC_CONFIG_HIDDEN;
        oldModuleConfig = m_hideModule;
        newModuleConfig = &m_hideModule;
    } else if (key == DisableConfig) {
        type = DCC_CONFIG_DISABLED;
        oldModuleConfig = m_disableModule;
        newModuleConfig = &m_disableModule;
    }
    if (!newModuleConfig)
        return;

    QString configValue = m_dconfig->value(key).toString();
    configValue.remove('[');
    configValue.remove(']');
    configValue.remove('\"');
    *newModuleConfig = QSet<QString>::fromList(configValue.split(",", QString::SkipEmptyParts));

    QSet<QString> addModuleConfig = findAddItems(&oldModuleConfig, newModuleConfig);
    QSet<QString> removeModuleConfig = findAddItems(newModuleConfig, &oldModuleConfig);
    for (auto &&url : addModuleConfig) {
        ModuleObject *obj = getModuleByUrl(m_rootModule, url, UrlType::Name);
        if (obj)
            obj->setFlagState(type, true);
    }
    for (auto &&url : removeModuleConfig) {
        ModuleObject *obj = getModuleByUrl(m_rootModule, url, UrlType::Name);
        if (obj)
            obj->setFlagState(type, false);
    }
}

void MainWindow::updateLayoutCurrent(LayoutBase *layout, ModuleObject *child)
{
    layout->setCurrent(child);
}

void MainWindow::loadModules()
{
    onAddModule(m_rootModule);
    m_pluginManager->loadModules(m_rootModule, m_layoutManager);
    showModule(m_rootModule, m_contentWidget);
    // 搜索没实时更新，插件并行加载，此处暂延时设置，待修改
    QTimer::singleShot(3000, this, [this]() {
        m_searchWidget->setModuleObject(m_rootModule);
    });
}

void MainWindow::toHome()
{
    showModule(m_rootModule, m_contentWidget);
}

void MainWindow::updateMainView()
{
    // 在布局中，待实现
    //    if (!m_mainView)
    //        return;
    //    // set background
    //    DPalette pa = DPaletteHelper::instance()->palette(m_mainView);
    //    QColor baseColor = palette().base().color();
    //    DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(baseColor);
    //    if (ct == DGuiApplicationHelper::LightType) {
    //        pa.setBrush(DPalette::ItemBackground, palette().base());
    //    } else {
    //        baseColor = DGuiApplicationHelper::adjustColor(baseColor, 0, 0, +5, 0, 0, 0, 0);
    //        pa.setColor(DPalette::ItemBackground, baseColor);
    //    }
    //    DPaletteHelper::instance()->setPalette(m_mainView, pa);
}

void MainWindow::clearPage(QWidget *const widget)
{
    QLayout *layout = widget->layout();
    QScrollArea *area = qobject_cast<QScrollArea *>(widget);
    if (area)
        area->widget()->deleteLater();

    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            if (child->widget())
                child->widget()->deleteLater();
            delete child;
        }
        delete layout;
    }
}

void MainWindow::configLayout(QBoxLayout *const layout)
{
    layout->setMargin(0);
    layout->setSpacing(0);
}

void MainWindow::showModule(ModuleObject *const module, QWidget *const parent)
{
    QList<ModuleObject *> modules;
    ModuleObject *obj = module;
    while (obj) { // ==root
        modules.prepend(obj);
        obj = dynamic_cast<ModuleObject *>(obj->parent());
    }

    // DEBUG===================
    {
        QStringList newList;
        QStringList oldList;
        for (auto &tmp : modules) {
            newList << tmp->name();
        }
        for (auto &tmp : m_currentModule) {
            oldList << tmp.module->name();
        }
        qInfo() << __LINE__ << oldList.join("/") << "=>" << newList.join("/");
    }
    // DEBUG===================

    QWidget *widget = parent;
    int i = 0;
    while (true) {
        // A/B=>A/B/C 从B开始布局
        if (m_currentModule.length() <= i) {
            if (i == 0 || modules.isEmpty())
                break;

            modules.prepend(m_currentModule.at(i - 1).module);
            widget = m_currentModule.at(i - 1).w;
            m_currentModule = m_currentModule.mid(0, i - 1);
            break;
        }
        if (modules.isEmpty()) {
            if (i == 0)
                break;
            ModuleObject *child = m_currentModule.at(i - 1).layout->autoExpand(m_currentModule.at(i - 1).module, modules);
            // A/B/C=>A/B 从B开始布局
            if (!child) {
                modules.prepend(m_currentModule.at(i - 1).module);
                widget = m_currentModule.at(i - 1).w;
                m_currentModule = m_currentModule.mid(0, i - 1);
                break;
            }
            return;
        }
        if (m_currentModule.at(i).module == modules.first()) {
            obj = modules.takeFirst();
        } else { // A/B/C=>A/B/D 从B开始布局
            widget = m_currentModule.at(i).w;
            m_currentModule = m_currentModule.mid(0, i);
            if (!m_currentModule.isEmpty()) {
                const WidgetData &data = m_currentModule.last();
                data.layout->setCurrent(modules.first());
            }
            break;
        }
        i++;
    }
    // DEBUG===================
    {
        QStringList newList;
        QStringList oldList;
        for (auto &tmp : modules) {
            newList << tmp->name();
        }
        for (auto &tmp : m_currentModule) {
            oldList << tmp.module->name();
        }
        qInfo() << __LINE__ << oldList.join("/") << "=>" << newList.join("/") << i;
    }
    // DEBUG===================

    while (widget && !modules.isEmpty()) {
        obj = modules.takeFirst();
        if (!obj)
            return;
        obj->active();
        clearPage(widget);

        LayoutBase *layout = m_layoutManager->createLayout(obj->childType());
        WidgetData data;
        data.w = widget;
        data.module = obj;
        data.layout = layout;
        m_currentModule.append(data);
        widget = layout->layoutModule(obj, widget, modules);
        if (modules.isEmpty()) {
            ModuleObject *child = layout->autoExpand(obj, modules);
            if (child)
                modules.append(child);
        }
        if (!modules.isEmpty())
            QMetaObject::invokeMethod(this, "updateLayoutCurrent", Qt::QueuedConnection, Q_ARG(LayoutBase *, layout), Q_ARG(ModuleObject *, modules.first()));
    }
}

ModuleObject *MainWindow::getModuleByUrl(ModuleObject *const root, const QString &url, const MainWindow::UrlType &uType)
{
    ModuleObject *obj = root;
    ModuleObject *parent = nullptr;
    QStringList names = url.split('/');
    while (!names.isEmpty() && obj) {
        const QString &name = names.takeFirst();
        QString childName;
        parent = obj;
        obj = nullptr;
        for (auto child : parent->childrens()) {
            switch (uType) {
            case UrlType::Name:
                if (child->name() == name)
                    obj = child;
                break;
            case UrlType::DisplayName:
                if (child->displayName() == name || child->contentText().contains(name))
                    obj = child;
                break;
            }
            if (obj)
                break;
        }
    }
    return names.isEmpty() ? obj : nullptr;
}

QString MainWindow::getUrlByModule(ModuleObject *const module)
{
    QStringList url;
    ModuleObject *obj = module;
    while (obj && obj != m_rootModule) {
        url.prepend(obj->name());
        obj = obj->getParent();
    }
    return url.join('/');
}

void MainWindow::onAddModule(ModuleObject *const module)
{
    QString url = getUrlByModule(module);

    QList<QPair<QString, ModuleObject *>> modules;
    modules.append({ url, module });
    while (!modules.isEmpty()) {
        auto it = modules.takeFirst();
        ModuleObject *obj = it.second;
        configModule(it.first, obj);
        connect(obj, &ModuleObject::appendedChild, this, &MainWindow::onAddModule);
        connect(obj, &ModuleObject::insertedChild, this, &MainWindow::onAddModule);
        connect(obj, &ModuleObject::removedChild, this, &MainWindow::onRemoveModule);
        connect(obj, &ModuleObject::triggered, this, &MainWindow::onTriggered);
        for (auto &&tmpObj : obj->childrens()) {
            modules.append({ it.first + "/" + tmpObj->name(), tmpObj });
        }
    }
}

void MainWindow::onRemoveModule(ModuleObject *const module)
{
    QList<ModuleObject *> modules;
    modules.append(module);
    while (!modules.isEmpty()) {
        ModuleObject *obj = modules.takeFirst();
        disconnect(obj, nullptr, this, nullptr);
        modules.append(obj->childrens());
    }
}

void MainWindow::onTriggered()
{
    QObject *qobj = sender();
    ModuleObject *module = qobject_cast<ModuleObject *>(sender());
    ModuleObject *obj = dynamic_cast<ModuleObject *>(sender());
    if (obj && qobj && module) {
        showModule(obj, m_contentWidget);
    }
}

void MainWindow::openManual()
{
    QString helpTitle;
    if (!m_currentModule.isEmpty())
        helpTitle = m_currentModule.last().module->name();
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
