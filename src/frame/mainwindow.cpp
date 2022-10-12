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
#include "mainmodule.h"
#include "utils.h"

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
#include <QJsonArray>
#include <QJsonDocument>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

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

MainWindow::MainWindow(bool async, QWidget *parent)
    : DMainWindow(parent)
    , m_backwardBtn(new DIconButton(QStyle::SP_ArrowBack, this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", ControlCenterConfig, QString(), this))
    , m_searchWidget(new SearchWidget(this))
    , m_rootModule(new MainModule(this))
    , m_pluginManager(new PluginManager(this))
    , m_loadAllFinished(false)
{
    qRegisterMetaType<ModuleObject *>("ModuleObject *");

    initUI();
    initConfig();
    loadModules(async);

    connect(m_searchWidget, &SearchWidget::notifySearchUrl, this, [this](const QString &url) {
        showPage(url, UrlType::DisplayName);
    });
    connect(m_pluginManager, &PluginManager::loadAllFinished, this, [this]() {
        m_loadAllFinished = true;
        // 搜索没实时更新，插件并行加载，此处在插件加载完后更新，待修改为实时更新
        m_searchWidget->setModuleObject(m_rootModule);
    });
}

MainWindow::~MainWindow()
{
    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, width());
        m_dconfig->setValue(HeightConfig, height());
    }
    resizeCurrentModule(0);
    //    delete m_layoutManager;
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

void MainWindow::showPage(const QString &url)
{
    if (m_loadAllFinished) {
        showPage(url, UrlType::Name);
        return;
    }
    QTimer::singleShot(10, this, [url, this] {
        showPage(url);
    });
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

    showModule(obj);
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
    setCentralWidget(m_rootModule->activePage());

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
    uint32_t type = DCC_CONFIG_HIDDEN;
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

    const auto &list = m_dconfig->value(key).toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    *newModuleConfig = QSet<QString>(list.begin(), list.end());
#else
    *newModuleConfig = QSet<QString>::fromList(list);
#endif
    QSet<QString> addModuleConfig = findAddItems(&oldModuleConfig, newModuleConfig);
    QSet<QString> removeModuleConfig = findAddItems(newModuleConfig, &oldModuleConfig);
    for (auto &&url : addModuleConfig) {
        ModuleObject *obj = DCC_NAMESPACE::GetModuleByUrl(m_rootModule, url);
        if (obj)
            obj->setFlagState(type, true);
    }
    for (auto &&url : removeModuleConfig) {
        ModuleObject *obj = DCC_NAMESPACE::GetModuleByUrl(m_rootModule, url);
        if (obj)
            obj->setFlagState(type, false);
    }
}

void MainWindow::loadModules(bool async)
{
    onAddModule(m_rootModule);
    m_pluginManager->loadModules(m_rootModule, async);
    showModule(m_rootModule);
    // 搜索没实时更新，插件并行加载，此处暂延时设置，待修改
    // QTimer::singleShot(3000, this, [this]() {
    //     m_searchWidget->setModuleObject(m_rootModule);
    // });
}

void MainWindow::toHome()
{
    showModule(m_rootModule);
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
    QLayout *widgetLayout = widget->layout();
    if (!widgetLayout)
        return;

    QList<QLayout *> layouts;
    layouts.append(widgetLayout);
    while (!layouts.isEmpty()) {
        QLayout *layout = layouts.takeFirst();
        while (!layout->isEmpty()) {
            QLayoutItem *item = layout->takeAt(0);
            QWidget *w = item->widget();
            if (w)
                delete w;
            QLayout *l = item->layout();
            if (l)
                layouts.append(l);
        }
        delete layout;
    }
}

void MainWindow::configLayout(QBoxLayout *const layout)
{
    layout->setMargin(0);
    layout->setSpacing(0);
}

void MainWindow::showModule(ModuleObject *const module)
{
    if (m_currentModule.contains(module) && module->defultModule())
        return;

    m_backwardBtn->setEnabled(module != m_rootModule);
    QList<ModuleObject *> modules;

    ModuleObject *child = module;
    while (child) {
        child->setCurrentModule(child->defultModule());
        modules.append(child);
        child = child->currentModule();
    }
    child = module;
    ModuleObject *p = module->getParent();
    while (p) {
        p->setCurrentModule(child);
        modules.prepend(p);
        child = p;
        p = p->getParent();
    }
    m_currentModule = modules;
}

void MainWindow::resizeCurrentModule(int size)
{
    m_currentModule = m_currentModule.mid(0, size);
}

void MainWindow::onAddModule(ModuleObject *const module)
{
    QString url = DCC_NAMESPACE::GetUrlByModule(module);

    QList<QPair<QString, ModuleObject *>> modules;
    modules.append({ url, module });
    while (!modules.isEmpty()) {
        auto it = modules.takeFirst();
        ModuleObject *obj = it.second;
        configModule(it.first, obj);
        connect(obj, &ModuleObject::insertedChild, this, &MainWindow::onAddModule);
        connect(obj, &ModuleObject::removedChild, this, &MainWindow::onRemoveModule);
        connect(obj, &ModuleObject::childStateChanged, this, &MainWindow::onChildStateChanged);
        connect(obj, &ModuleObject::triggered, this, &MainWindow::onTriggered, Qt::QueuedConnection);
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
    // 最后一个是滚动到，不参与比较
    if (!m_currentModule.isEmpty() && std::any_of(m_currentModule.cbegin(), m_currentModule.cend() - 1, [module](auto &&data) { return data == module; }))
        toHome();
}

void MainWindow::onTriggered()
{
    QObject *qobj = sender();
    ModuleObject *module = qobject_cast<ModuleObject *>(sender());
    ModuleObject *obj = dynamic_cast<ModuleObject *>(sender());
    if (obj && qobj && module) {
        showModule(obj);
    }
}

void MainWindow::onChildStateChanged(ModuleObject *const child, uint32_t flag, bool state)
{
    if (ModuleObject::IsHiddenFlag(flag)) {
        if (state)
            onRemoveModule(child);
        else
            onAddModule(child);
    }
}

void MainWindow::openManual()
{
    QString helpTitle;
    if (!m_currentModule.isEmpty())
        helpTitle = m_currentModule.last()->name();
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

QString MainWindow::getAllModule() const
{
    ModuleObject *root = m_rootModule;
    QList<QPair<ModuleObject *, QStringList>> modules;
    for (auto &&child : root->childrens()) {
        modules.append({ child, { child->name(), child->displayName() } });
    }

    QJsonArray arr;
    while (!modules.isEmpty()) {
        const auto &urlInfo = modules.takeFirst();
        QJsonObject obj;
        obj.insert("url", urlInfo.second.at(0));
        obj.insert("displayName", urlInfo.second.at(1));
        arr.append(obj);
        const QList<ModuleObject *> &children = urlInfo.first->childrens();
        for (auto it = children.crbegin(); it != children.crend(); ++it)
            modules.prepend({ *it, { urlInfo.second.at(0) + "/" + (*it)->name(), urlInfo.second.at(1) + "/" + (*it)->displayName() } });
    }

    QJsonDocument doc;
    doc.setArray(arr);
    return doc.toJson(QJsonDocument::Compact);
}
