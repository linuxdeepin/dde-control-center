//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include "utils.h"

#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QtConcurrent>
#include <QFileInfo>
#include <QSettings>
#include <queue>

using namespace DCC_NAMESPACE;

const QString &PluginDirectory = QStringLiteral(DefaultModuleDirectory);

bool compareVersion(const QString &targetVersion, const QString &baseVersion)
{
    const QStringList &version1 = baseVersion.split(".");
    const QStringList &version2 = targetVersion.split(".");

    const auto size = qAbs<int>(version1.size() - version2.size());

    for (int i = 0; i < size; ++i) {
        // 相等判断下一个子版本号
        if (version1[i] == version2[i])
            continue;

        // 转成整形比较
        if (version1[i].toInt() > version2[i].toInt()) {
            return false;
        } else {
            return true;
        }
    }

    return true;
}

bool comparePluginLocation(PluginInterface const *target1, PluginInterface const *target2)
{
    return target1->location() < target2->location();
}

PluginData getModule(const QPair<PluginManager *, PluginData> &pair)
{
    PluginData data = pair.second;
    if (data.Plugin) {
        QElapsedTimer et;
        et.start();
        data.Module = data.Plugin->module();
        data.Module->setParent(nullptr);
        data.Module->moveToThread(qApp->thread());
        qInfo() << QString("get module: %1 end, using time: %2 ms").arg(data.Module->name()).arg(et.elapsed());
        emit pair.first->loadedModule(data);
    }
    return data;
}

PluginData loadPlugin(const QPair<PluginManager *, QString> &pair)
{
    PluginData data;
    data.Plugin = nullptr;
    data.Module = nullptr;
    data.Location = -1;
    auto &&fileName = pair.second;
    QFileInfo fileInfo(fileName);
    QSettings settings(DCC_NAMESPACE::CollapseConfgPath, QSettings::IniFormat);
    settings.beginGroup("collapse");
    const QByteArray &md5 = settings.value(fileInfo.fileName()).toByteArray();
    settings.endGroup();
    if (DCC_NAMESPACE::getFileMd5(fileName).toHex() == md5) {
        qWarning() << QString("The Plugin: %1 crashed, will not load!").arg(fileName);
        return data;
    }
    qInfo() << "loading plugin: " << fileName;

    QElapsedTimer et;
    et.start();
    QScopedPointer<QPluginLoader> loader(new QPluginLoader(fileName));
    if (!loader->load()) {
        qWarning() << QString("The plugin: %1 load failed! error message: %2").arg(fileName, loader->errorString());
        return data;
    }
    const QJsonObject &meta = loader->metaData().value("MetaData").toObject();
    if (!compareVersion(meta.value("api").toString(), "1.0.0")) {
        qWarning() << QString("The version of plugin: %1 is too low!").arg(fileName);
        return data;
    }

    PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
    if (!plugin) {
        qWarning() << QString("Can't read plugin: %1").arg(fileName);
        loader->unload();
        return data;
    }

    data.Plugin = plugin;
    data.Follow = plugin->follow();
    data.Location = plugin->location();
    data.Plugin->setParent(nullptr);
    data.Plugin->moveToThread(qApp->thread());
    qInfo() << QString("load plugin: %1 end, using time: %2 ms").arg(fileName).arg(et.elapsed());
    return data;
}

PluginData loadModule(const QPair<PluginManager *, QString> &pair)
{
    return loadPlugin(pair);
}

PluginData loadAndGetModule(const QPair<PluginManager *, QString> &pair)
{
    PluginData &&data = loadPlugin(pair);
    if (data.Plugin) {
        getModule({ pair.first, data });
    }
    return data;
}

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_rootModule(nullptr)
    , m_loadAllFinished(false)
{
    qRegisterMetaType<PluginData>("PluginData");
}

void PluginManager::loadModules(ModuleObject *root, bool async)
{
    if (!root)
        return;
    m_rootModule = root;

    connect(this, &PluginManager::loadedModule, root, [this](const PluginData &data) {
        initModules(data);
    });

    QDir pluginDir(PluginDirectory);
#ifdef QT_DEBUG
    pluginDir.setPath(qApp->applicationDirPath());
#endif
    if (!pluginDir.exists()) {
        qWarning() << "plugin directory not exists";
        return;
    }

    auto &&pluginList = pluginDir.entryInfoList();
    QList<QPair<PluginManager *, QString>> libraryNames;
    for (auto &&lib : pluginList) {
        auto &&fileName = lib.absoluteFilePath();
        if (!QLibrary::isLibrary(fileName))
            continue;
        libraryNames.append({ this, fileName });
    }

    QFutureWatcher<PluginData> *watcher = new QFutureWatcher<PluginData>(this);
    if (async) {
        m_future = QtConcurrent::mapped(libraryNames, loadAndGetModule);
        connect(watcher, &QFutureWatcher<PluginData>::finished, this, [this] {
            // 加载非一级插件
            insertChild(true);
            m_loadAllFinished = true;
            emit loadAllFinished();
        });
        watcher->setFuture(m_future);
    } else {
        QFuture<PluginData> future = QtConcurrent::mapped(libraryNames, loadModule);
        watcher->waitForFinished();
        future.results();
        QList<QPair<PluginManager *, PluginData>> pluginDatas;
        for (auto &&data : future.results()) {
            pluginDatas.append({ this, data });
        }
        m_future = QtConcurrent::mapped(pluginDatas, getModule);
        connect(watcher, &QFutureWatcher<PluginData>::finished, this, [this] {
            // 加载非一级插件
            insertChild(true);
            m_loadAllFinished = true;
            emit loadAllFinished();
        });
        watcher->setFuture(m_future);
    }
}

void PluginManager::cancelLoad()
{
    if (!loadFinished()) {
        m_future.cancel();
    }
}

bool PluginManager::loadFinished() const
{
    return m_loadAllFinished;
}

ModuleObject *PluginManager::findModule(ModuleObject *module, const QString &name)
{
    if (!module)
        return nullptr;

    std::queue<ModuleObject *> qbfs;
    qbfs.push(module);
    while (!qbfs.empty()) {
        if (qbfs.front()->name() == name)
            return qbfs.front();
        for (auto &&child : qbfs.front()->childrens()) {
            qbfs.push(child);
        }
        qbfs.pop();
    }

    return nullptr;
}

void PluginManager::initModules(const PluginData &data)
{
    if (data.Follow.isEmpty()) { // root plugin
        data.Module->setProperty("location", data.Location);
        if (!m_rootModule->hasChildrens()) {
            m_rootModule->appendChild(data.Module);
            insertChild(false);
            return;
        }
        if (data.Location.isEmpty()) {
            m_rootModule->appendChild(data.Module);
            insertChild(false);
            return;
        }
        bool isInt;
        int curLocation = data.Location.toInt(&isInt);

        int i = m_rootModule->childrens().count() - 1;
        for (; i >= 0; i--) {
            if (isInt) {
                bool ok = false;
                const QString &location = m_rootModule->childrens().at(i)->property("location").toString();
                int tmpLocation = location.toInt(&ok);
                if (ok && tmpLocation >= 0 && curLocation > tmpLocation) {
                    break;
                }
            } else {
                if (m_rootModule->children(i)->name() == data.Location)
                    break;
            }
        }
        m_rootModule->insertChild(i + 1, data.Module);
        insertChild(false);
    } else { // other plugin
        m_datas.append(data);
    }
}

void PluginManager::insertChild(bool force)
{
    bool loop = true;
    while (loop) {
        loop = false;
        for (auto it = m_datas.begin(); it != m_datas.end();) {
            auto &&module = DCC_NAMESPACE::GetModuleByUrl(m_rootModule, it->Follow);
            if (module) {
                bool isInt;
                int locationIndex = it->Location.toInt(&isInt);
                if (!isInt) {
                    for (locationIndex = 0; locationIndex < module->getChildrenSize(); ++locationIndex) {
                        if (module->children(locationIndex)->name() == it->Location) {
                            ++locationIndex;
                            break;
                        }
                    }
                }
                module->insertChild(locationIndex, it->Module);
                loop = true;
                it = m_datas.erase(it);
            } else
                ++it;
        }
    }
    if (force) {
        // 释放加不进去的module
        for (auto &&data : m_datas) {
            qWarning() << "Unkown Module! name:" << data.Module->name() << "follow:" << data.Follow << "location:" << data.Location;
            delete data.Module;
        }
        m_datas.clear();
    }
}
