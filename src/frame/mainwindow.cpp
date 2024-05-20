// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "mainwindow.h"

#include "interface/moduleobject.h"
#include "mainmodule.h"
#include "pluginmanager.h"
#include "searchwidget.h"
#include "utils.h"
#include "widgets/utils.h"

#include <DBackgroundGroup>
#include <DButtonBox>
#include <DConfig>
#include <DFrame>
#include <DGuiApplicationHelper>
#include <DIconButton>
#include <DIconTheme>
#include <DListView>
#include <DPalette>
#include <DPaletteHelper>
#include <DPushButton>
#include <DTitlebar>
#include <DWidgetUtil>

#include <QColor>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QShortcutEvent>
#include <QStandardItemModel>
#include <QTimer>

DCORE_USE_NAMESPACE
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

#define DCC_CONFIG_HIDDEN 0x20000000
#define DCC_CONFIG_DISABLED 0x10000000
#define setConfigHidden(hide) setFlagState(DCC_CONFIG_HIDDEN, hide)
#define setConfigDisabled(disabled) setFlagState(DCC_CONFIG_DISABLED, disabled)

constexpr QSize MainWindowMininumSize(QSize(800, 600));

const QString ControlCenterConfig = QStringLiteral("org.deepin.dde.control-center");
const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");
const QString HideConfig = QStringLiteral("hideModule");
const QString DisableConfig = QStringLiteral("disableModule");
const QString ControlCenterIcon = QStringLiteral("preferences-system");
// 这个只有 com.xx 全局搜索才翻译设置
const QString ControlCenterGroupName =
        "com.deepin.dde-grand-search.group.dde-control-center-setting";

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
    , m_backwardBtn(new DIconButton(QStyle::SP_ArrowBack, this))
    , m_dconfig(DConfig::create(
              "org.deepin.dde.control-center", ControlCenterConfig, QString(), this))
    , m_searchWidget(new SearchWidget(this))
    , m_rootModule(new MainModule(this))
    , m_pluginManager(new PluginManager(this))
{
    qRegisterMetaType<ModuleObject *>("ModuleObject *");

    initUI();
    initConfig();

    connect(m_searchWidget, &SearchWidget::notifySearchUrl, this, [this](const QString &url) {
        showPage(url, UrlType::Name);
    });
    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    int width_remember = width() > 1000 ? width() : 1000;
    int height_remember = height() > 600 ? height() : 600;

    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, width_remember);
        m_dconfig->setValue(HeightConfig, height_remember);
    }
    resizeCurrentModule(0);
}

void MainWindow::showPage(const QString &url, const UrlType &uType)
{
    if (qApp->activeModalWidget()) {
        qInfo() << "controlcenter has modal dialog, cannot switch page";
        return;
    }
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
    if (findModule(m_rootModule, url, UrlType::Name, false)) {
        showPage(url, UrlType::Name);
        return;
    }
    QTimer::singleShot(10, this, [url, this] {
        showPage(url);
    });
}

void MainWindow::showPage(ModuleObject *const module, const QString &url, const UrlType &uType)
{
    showModule(findModule(module, url, uType));
}

ModuleObject *MainWindow::findModule(ModuleObject *const module,
                                     const QString &url,
                                     const UrlType &uType,
                                     bool fuzzy)
{
    ModuleObject *obj = module;
    QStringList names = url.split('/');
    while (!names.isEmpty() && obj) {
        const QString &name = names.takeFirst();
        QString childName;
        ModuleObject *objChild = nullptr;
        for (auto child : obj->childrens()) {
            switch (uType) {
            case UrlType::DisplayName:
                childName = child->displayName();
                break;
            default:
                childName = child->name();
                break;
            }
            if (childName == name || child->contentText().contains(name)) {
                objChild = child;
                break;
            }
        }
        if (objChild) {
            obj = objChild;
        } else if (fuzzy) {
            break;
        } else {
            return nullptr;
        }
    }
    return obj;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateMainView();
    }
    return DMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    m_pluginManager->cancelLoad();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Shortcut) {
        auto ke = static_cast<QShortcutEvent *>(event);
        if (ke->key() == Qt::Key_F1) {
            openManual();
            return true;
        }
    }
    return DMainWindow::eventFilter(watched, event);
}

void MainWindow::initUI()
{
    setMinimumSize(MainWindowMininumSize);
    auto root = m_rootModule->activePage();
    root->setAutoFillBackground(true);
    setCentralWidget(root);

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
    m_backwardBtn->setFlat(true);
    // flat iconButton size too small
    m_backwardBtn->setMinimumSize({32, 32});

    titlebar()->addWidget(m_backwardBtn, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar()->setIcon(DIconTheme::findQIcon("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, &MainWindow::toHome);

    m_searchWidget->setMinimumSize(300, 36);
    m_searchWidget->setAccessibleName("SearchModule");
    m_searchWidget->lineEdit()->setAccessibleName("SearchModuleLineEdit");
    titlebar()->addWidget(m_searchWidget, Qt::AlignCenter);
}

void MainWindow::initConfig()
{
    if (!m_dconfig->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].")
                              .arg(m_dconfig->name(), m_dconfig->subpath());
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

void MainWindow::loadModules(bool async, const QStringList &dirs)
{
    onAddModule(m_rootModule);
    m_pluginManager->loadModules(m_rootModule, async, dirs);
    showModule(m_rootModule);
}

QString MainWindow::GrandSearchSearch(const QString json)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();

        // 处理搜索任务, 返回搜索结果
        QList<QPair<QString, QString>> lstMsg =
                m_searchWidget->searchResults(jsonObject.value("cont").toString());

        for (auto msg : lstMsg) {
            qDebug() << "name:" << msg;
        }

        QJsonObject jsonResults;
        QJsonArray items;
        for (int i = 0; i < lstMsg.size(); i++) {
            QJsonObject jsonObj;
            jsonObj.insert("item", lstMsg[i].first);
            jsonObj.insert("name", lstMsg[i].second);
            jsonObj.insert("icon", ControlCenterIcon);
            jsonObj.insert("type", "application/x-dde-control-center-xx");

            items.insert(i, jsonObj);
        }

        QJsonObject objCont;
        objCont.insert("group", ControlCenterGroupName);
        objCont.insert("items", items);

        QJsonArray arrConts;
        arrConts.insert(0, objCont);

        jsonResults.insert("ver", jsonObject.value("ver"));
        jsonResults.insert("mID", jsonObject.value("mID"));
        jsonResults.insert("cont", arrConts);

        QJsonDocument document;
        document.setObject(jsonResults);

        return document.toJson(QJsonDocument::Compact);
    }

    return QString();
}

bool MainWindow::GrandSearchStop(const QString &json)
{
    Q_UNUSED(json)
    return true;
}

bool MainWindow::GrandSearchAction(const QString &json)
{
    QString searchName;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        if (jsonObject.value("action") == "openitem") {
            // 打开item的操作
            searchName = jsonObject.value("item").toString();
        }
    }

    show();
    activateWindow();
    showPage(searchName, UrlType::Name);
    return true;
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

    m_backwardBtn->setVisible(module != m_rootModule);
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
        connect(obj, &ModuleObject::moduleDataChanged, this, &MainWindow::onModuleDataChanged);
        connect(obj,
                &ModuleObject::triggered,
                this,
                &MainWindow::onTriggered,
                Qt::QueuedConnection);
        m_searchWidget->addModule(obj);
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
        m_searchWidget->removeModule(obj);
        modules.append(obj->childrens());
    }
    // 最后一个是滚动到，不参与比较
    if (!m_currentModule.isEmpty()
        && std::any_of(m_currentModule.cbegin(), m_currentModule.cend() - 1, [module](auto &&data) {
               return data == module;
           }))
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

void MainWindow::onModuleDataChanged()
{
    ModuleObject *module = qobject_cast<ModuleObject *>(sender());
    if (module) {
        m_searchWidget->removeModule(module);
        m_searchWidget->addModule(module);
    }
}

void MainWindow::openManual()
{
    QString helpTitle;
    if (1 < m_currentModule.count())
        helpTitle = m_currentModule[1]->name();
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
            modules.prepend({ *it,
                              { urlInfo.second.at(0) + "/" + (*it)->name(),
                                urlInfo.second.at(1) + "/" + (*it)->displayName() } });
    }

    QJsonDocument doc;
    doc.setArray(arr);
    return doc.toJson(QJsonDocument::Compact);
}
