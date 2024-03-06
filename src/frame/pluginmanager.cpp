// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pluginmanager.h"

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include "utils.h"

#include <DGuiApplicationHelper>
#include <DNotifySender>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QSet>
#include <QSettings>
#include <QtConcurrent>
#include <QtConcurrentRun>

Q_LOGGING_CATEGORY(DdcFramePluginManager, "dcc-frame-pluginManager")

#include <mutex>
#include <queue>

std::mutex PLUGIN_LOAD_GUARD;

using namespace DCC_NAMESPACE;

const static QString TranslateReadDir = QStringLiteral(TRANSLATE_READ_DIR);

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
        qCInfo(DdcFramePluginManager) << QString("get module: %1 end, using time: %2 ms")
                                                 .arg(data.Module->name())
                                                 .arg(et.elapsed());
        emit pair.first->loadedModule(data);
    }
    return data;
}

PluginData loadPlugin(const QPair<PluginManager *, QString> &pair)
{
    PluginData data;
    data.Plugin = nullptr;
    data.Module = nullptr;
    data.Location = "-1";
    auto &&fileName = pair.second;
    QFileInfo fileInfo(fileName);
    QSettings settings(DCC_NAMESPACE::CollapseConfgPath, QSettings::IniFormat);
    settings.beginGroup("collapse");
    const QByteArray &md5 = settings.value(fileInfo.fileName()).toByteArray();
    settings.endGroup();
    if (DCC_NAMESPACE::getFileMd5(fileName).toHex() == md5) {
        qCWarning(DdcFramePluginManager)
                << QString("The Plugin: %1 crashed, will not load!").arg(fileName);
        return data;
    }
    qCInfo(DdcFramePluginManager) << "loading plugin: " << fileName;

    QElapsedTimer et;
    et.start();
    QScopedPointer<QPluginLoader> loader(new QPluginLoader(fileName));
    if (!loader->load()) {
        qCWarning(DdcFramePluginManager) << QString("The plugin: %1 load failed! error message: %2")
                                                    .arg(fileName, loader->errorString());
        return data;
    }
    const QJsonObject &meta = loader->metaData().value("MetaData").toObject();

    PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
    if (!plugin) {
        qCWarning(DdcFramePluginManager) << QString("Can't read plugin: %1").arg(fileName);
        loader->unload();
        return data;
    }

    // FIXME: load all plugin under treeland
    static QByteArray compositor = qgetenv("DDE_CURRENT_COMPOSITOR");
    static QStringList allowedUnderTreeland{ "accounts",
                                             "display",
                                             "systeminfo",
                                             "Default Applications" };

    if (compositor.compare("treeland", Qt::CaseInsensitive) == 0
        and !allowedUnderTreeland.contains(plugin->name())) {
        qCWarning(DdcFramePluginManager)
                << QString("plugin %1 has been banned under treeland.").arg(plugin->name());
        loader->unload();
        return data;
    }

    data.Plugin = plugin;
    data.Follow = plugin->follow();
    data.Location = plugin->location();
    data.Plugin->setParent(nullptr);
    data.Plugin->moveToThread(qApp->thread());
    qCInfo(DdcFramePluginManager)
            << QString("load plugin: %1 end, using time: %2 ms").arg(fileName).arg(et.elapsed());
    return data;
}

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_rootModule(nullptr)
{
    qRegisterMetaType<PluginData>("PluginData");
}

void PluginManager::loadModules(ModuleObject *root, bool async, const QStringList &dirs)
{
    if (!root)
        return;
    m_rootModule = root;

    connect(this, &PluginManager::loadedModule, root, [this](const PluginData &data) {
        initModules(data);
    });

    QFileInfoList pluginList;
    for (const auto &dir : dirs) {
        QDir plugindir(dir);
        if (plugindir.exists()) {
            pluginList += plugindir.entryInfoList();
        }
    }

    QSet<QString> filenames;
    QList<QPair<PluginManager *, QString>> libraryNames;
    for (auto &lib : pluginList) {
        const QString &filepath = lib.absoluteFilePath();
        if (!QLibrary::isLibrary(filepath)) {
            continue;
        }
        auto filename = lib.fileName();
        // 不加载文件名重复的模块
        if (filenames.contains(filename)) {
            continue;
        }
        filenames.insert(filename);
        libraryNames.append({ this, filepath });
        m_pluginsStatus.push_back(filepath);
    }

    QFutureWatcher<PluginData> *watcher = new QFutureWatcher<PluginData>(this);
    std::function<QPair<PluginData, QString>(const QPair<PluginManager *, QString> &pair)>
            loadPluginAndRecord = [](const QPair<PluginManager *, QString> &pair) {
                auto plugin = loadPlugin(pair);
                return QPair{ plugin, pair.second };
            };
    auto results = QtConcurrent::mapped(libraryNames, loadPluginAndRecord).results();
    using Dtk::Gui::DGuiApplicationHelper;
    for (const auto &re : results) {
        if (re.first.Plugin) {
            QString qmFile = QString("%1/%2_%3.qm")
                                     .arg(TranslateReadDir)
                                     .arg(re.first.Plugin->name())
                                     .arg(QLocale::system().name());
            if (!QFile::exists(qmFile)) {
                continue;
            }
            DGuiApplicationHelper::loadTranslator(re.first.Plugin->name(),
                                                  { TranslateReadDir },
                                                  QList<QLocale>() << QLocale::system());
        }
    }
    std::function<PluginData(const QPair<PluginData, QString>)> loadModuleAndRecord =
            [this](const QPair<PluginData, QString> &data) {
                if (data.first.Plugin) {
                    getModule({ this, data.first });
                }
                std::lock_guard<std::mutex> guard(PLUGIN_LOAD_GUARD);
                m_pluginsStatus.remove(m_pluginsStatus.indexOf(data.second));
                return data.first;
            };
    m_future = QtConcurrent::mapped(results, loadModuleAndRecord);
    connect(watcher, &QFutureWatcher<PluginData>::finished, this, [this] {
        // 加载非一级插件
        insertChild(true);
        emit loadAllFinished();
    });
    watcher->setFuture(m_future);

    QtConcurrent::run([this] {
        // NOTE: wait for all plugin for 10 seconds, if timeout and run with sync
        // then watcher will be finished
        QThread::sleep(10);
        std::lock_guard<std::mutex> guard(PLUGIN_LOAD_GUARD);
        if (!m_pluginsStatus.isEmpty()) {
            QString logMessage = "Some plugins not loaded in time: ";
            for (const QString &value : m_pluginsStatus) {
                logMessage.push_back(value.split('/').last());
                logMessage.push_back(";");
            }
            qCWarning(DdcFramePluginManager) << logMessage;
        }
    });
    if (!async) {
        for (int i = 0; i < 50; i++) {
            QThread::msleep(100);
            if (m_pluginsStatus.isEmpty()) {
                break;
            }
        }
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
    return m_pluginsStatus.isEmpty();
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
                const QString &location =
                        m_rootModule->childrens().at(i)->property("location").toString();
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
                    for (locationIndex = 0; locationIndex < module->getChildrenSize();
                         ++locationIndex) {
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
            qCWarning(DdcFramePluginManager)
                    << "Unknown Module! name:" << data.Module->name() << "follow:" << data.Follow
                    << "location:" << data.Location;
            delete data.Module;
        }
        m_datas.clear();
    }
}
