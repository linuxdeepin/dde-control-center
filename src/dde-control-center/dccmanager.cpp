// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccmanager.h"

#include "dccapp.h"
#include "pluginmanager.h"
#include "dccobject_p.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>

DCORE_USE_NAMESPACE

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.manager");

const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");
const QString HideConfig = QStringLiteral("hideModule");
const QString DisableConfig = QStringLiteral("disableModule");

DccManager::DccManager(QObject *parent)
    : DccApp(parent)
    , m_root(new DccObject(this))
    , m_activeObject(m_root)
    , m_plugins(new PluginManager(this))
    , m_window(nullptr)
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center", QString(), this))
    , m_engine(nullptr)
{
    m_root->setName("root");
    m_root->setDefultObject(nullptr);
    m_currentObjects.append(m_root);

    initConfig();
    connect(m_root, &DccObject::triggered, this, &DccManager::onTriggered, Qt::QueuedConnection);
    connect(m_plugins, &PluginManager::addObject, this, &DccManager::addObject, Qt::QueuedConnection);
}

DccManager::~DccManager()
{
    int width_remember = m_window->width() > 1000 ? m_window->width() : 1000;
    int height_remember = m_window->height() > 600 ? m_window->height() : 600;

    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, width_remember);
        m_dconfig->setValue(HeightConfig, height_remember);
    }
    // delete m_engine;
    // m_engine = nullptr;
    delete m_root;
}

void DccManager::init()
{
    if (m_engine)
        return;

    m_engine = new QQmlApplicationEngine(this);
    auto paths = m_engine->importPathList();
    paths.prepend(DefaultModuleDirectory);
    m_engine->setImportPathList(paths);
}

QQmlApplicationEngine *DccManager::engine()
{
    return m_engine;
}

void DccManager::setMainWindow(QWindow *window)
{
    m_window = window;
}

void DccManager::loadModules(bool async, const QStringList &dirs)
{
    // onAddModule(m_rootModule);
    m_plugins->loadModules(m_root, async, dirs);
    // showModule(m_rootModule);
}

int DccManager::width() const
{
    auto w = m_dconfig->value(WidthConfig).toInt();
    return w > 1000 ? w : 1000;
}

int DccManager::height() const
{
    auto h = m_dconfig->value(HeightConfig).toInt();
    return h > 600 ? h : 600;
}

QString DccManager::path() const
{
    QStringList path;
    QString url;
    for (auto obj : m_currentObjects) {
        url += "/" + obj->name();
        if (!obj->displayName().isEmpty()) {
            path.append(QString("<a style=\"text-decoration: none;\" href=\"%1\">%2</a>").arg(url).arg(obj->displayName()));
        }
    }
    return path.join(" / ");
}

DccObject *DccManager::object(const QString &name)
{
    return nullptr;
}

void DccManager::addObject(DccObject *obj)
{
    if (!obj)
        return;
    QVector<DccObject *> objs;
    objs.append(obj);
    while (!objs.isEmpty()) {
        DccObject *o = objs.takeFirst();
        if (o->parentName().isEmpty()) {
            m_noParentObject.append(o);
        } else if (DccObject *parentObj = getObjectByUrl(m_root, o->parentName())) {
            DccObject::Private::FromObject(parentObj)->addChild(o);
            connect(o, &DccObject::triggered, this, &DccManager::onTriggered, Qt::QueuedConnection);
        } else {
            m_noAddObject.append(o);
        }

        objs.append(DccObject::Private::FromObject(o)->getObjects());
    }
    // 处理m_noAddObject
    objs.append(m_noAddObject);
    while (!objs.isEmpty()) {
        DccObject *o = objs.takeFirst();
        if (DccObject *parentObj = getObjectByUrl(m_root, o->parentName())) {
            DccObject::Private::FromObject(parentObj)->addChild(o);
            connect(o, &DccObject::triggered, this, &DccManager::onTriggered, Qt::QueuedConnection);
            m_noAddObject.removeOne(o);
            objs = m_noAddObject;
        }
    }
}

void DccManager::removeObject(DccObject *obj)
{
    if (!obj)
        return;
}

void DccManager::removeObject(const QString &name)
{
    removeObject(getObjectByUrl(m_root, name));
}

void DccManager::showPage(const QString &url)
{
    qWarning() << __FUNCTION__ << __LINE__ << url;
    if (url.isEmpty()) {
        showPage(m_root, QString());
    } else {
        int i = url.indexOf('.');
        QString cmd = i != -1 ? url.mid(i + 1, -1) : QString();
        qWarning() << __FUNCTION__ << __LINE__ << url.mid(0, i) << cmd;
        showPage(getObjectByUrl(m_root, url.mid(0, i)), cmd);
    }
}

void DccManager::showPage(DccObject *obj, const QString &cmd)
{
    if (!obj || (m_activeObject == obj && cmd.isEmpty()))
        return;
    // m_backwardBtn->setVisible(obj != m_root);
    QList<DccObject *> modules;
    DccObject *tmpObj = obj;
    while (tmpObj && (tmpObj->pageType() & DccObject::Control)) { // 页面中的控件，则激活项为父项
        tmpObj = DccObject::Private(tmpObj).getParent();
    }
    if (!tmpObj) {
        return;
    }
    modules.append(tmpObj);
    DccObject *p = DccObject::Private::FromObject(tmpObj)->getParent();
    while (p) {
        p->setCurrentObject(tmpObj);
        Q_EMIT p->active(QString());
        modules.prepend(p);
        tmpObj = p;
        p = DccObject::Private::FromObject(p)->getParent();
    }
    while (!m_currentObjects.isEmpty()) {
        DccObject *oldObj = m_currentObjects.takeLast();
        if (!modules.contains(oldObj)) {
            oldObj->setCurrentObject(nullptr);
        }
    }
    if (!cmd.isEmpty()) {
        Q_EMIT obj->active(cmd);
    }
    m_currentObjects = modules;
    qCWarning(dccLog) << m_currentObjects;
    if (m_currentObjects.last() != m_activeObject) {
        m_activeObject = m_currentObjects.last();
        Q_EMIT activeObjectChanged(m_activeObject);
    }
    Q_EMIT pathChanged(path());
    qWarning()<<__FUNCTION__<<m_activeObject<<m_activeObject->getChildren();
}

QWindow *DccManager::mainWindow()
{
    return m_window;
}

void DccManager::setShowPath(const QString &path) { }

QString DccManager::search(const QString json)
{
    return QString();
}

bool DccManager::stop(const QString json)
{
    return true;
}

bool DccManager::action(const QString json)
{
    return true;
}

void DccManager::initConfig()
{
    if (!m_dconfig->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_dconfig->name(), m_dconfig->subpath());
        return;
    }

    updateModuleConfig(HideConfig);
    updateModuleConfig(DisableConfig);
    connect(m_dconfig, &DConfig::valueChanged, this, &DccManager::updateModuleConfig);
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

DccObject *DccManager::getObjectByUrl(DccObject *const root, const QString &url)
{
    DccObject *target = nullptr;
    QStringList names = url.split('/', Qt::SkipEmptyParts);
    // if (!names.isEmpty() && names.first() == root->name()) {
    //     names.takeFirst();
    // }
    QVector<DccObject *> objs;
    objs.append(root);
    while (!names.isEmpty() && !objs.isEmpty()) {
        const QString &name = names.takeFirst();
        while (!objs.isEmpty()) {
            DccObject *obj = objs.takeFirst();
            if (obj->name() == name) {
                if (names.isEmpty()) {
                    target = obj;
                } else {
                    objs = obj->getChildren();
                }
                break;
            } else {
                objs.append(obj->getChildren());
            }
        }
    }
    return names.isEmpty() ? target : nullptr;
}

void DccManager::updateModuleConfig(const QString &key)
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
    } else {
        return;
    }
    const auto &list = m_dconfig->value(key).toStringList();
    *newModuleConfig = QSet<QString>(list.begin(), list.end());
    QSet<QString> addModuleConfig = findAddItems(&oldModuleConfig, newModuleConfig);
    QSet<QString> removeModuleConfig = findAddItems(newModuleConfig, &oldModuleConfig);
    for (auto &&url : addModuleConfig) {
        DccObject *obj = getObjectByUrl(m_root, url);
        if (obj)
            DccObject::Private::FromObject(obj)->setFlagState(type, true);
    }
    for (auto &&url : removeModuleConfig) {
        DccObject *obj = getObjectByUrl(m_root, url);
        if (obj)
            DccObject::Private::FromObject(obj)->setFlagState(type, false);
    }
}

void DccManager::onTriggered()
{
    DccObject *obj = qobject_cast<DccObject *>(sender());
    if (obj) {
        showPage(obj, QString());
    }
}

} // namespace dccV25
