// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appmgr.h"

#include "applicationinterface.h"
#include "objectmanager1interface.h"

#include <DConfig>
#include <DExpected>
DCORE_USE_NAMESPACE

using AppManager1Application = AppManager1Application;
using AppManager1ApplicationObjectManager = AppManager1ApplicationObjectManager;

static QString parseDisplayName(const QStringMap &source)
{
    static QString key = QLocale::system().name();
    return source.value(key, source.value(u8"default"));
}

static QString parseName(const QStringMap &source)
{
    QString name = source.value(QLocale::system().name());
    return !name.isEmpty() ? name : source.value(u8"default");
}

static QString parseIcon(const QStringMap &source)
{
    return source.value(u8"Desktop Entry");
}

template<class T>
static DExpected<T> parseDBusField(const QVariantMap &map, const QString &key)
{
    if (!map.contains(key))
        return {};
    const auto value = map.value(key);
    return DExpected<T>{qdbus_cast<T>(value)};
}

static QString getDisplayName(const bool isDeepin, const QStringMap &name, const QStringMap &genericName)
{
    if (isDeepin) {
        const auto tmp = parseDisplayName(genericName);
        if (!tmp.isEmpty())
            return tmp;
    }

    return parseDisplayName(name);
}

static AppMgr::AppItem *parseDBus2AppItem(const ObjectInterfaceMap &source)
{
    const QVariantMap appInfo = source.value("org.desktopspec.ApplicationManager1.Application");
    if (appInfo.isEmpty())
        return nullptr;

    const auto nodisplay = parseDBusField<bool>(appInfo, u8"NoDisplay");
    if (!nodisplay || nodisplay.value()) {
        return nullptr;
    }
    AppMgr::AppItem *item = nullptr;
    if (auto value = parseDBusField<QString>(appInfo, u8"ID")) {
        item = new AppMgr::AppItem();
        item->id = value.value() + ".desktop";
        item->appId = value.value();
    }

    if (!item) {
        return nullptr;
    }

    if (auto value = parseDBusField<QStringList>(appInfo, u8"Categories")) {
        item->categories = value.value();
    }

    // fallback to Name if GenericName is empty, only for X_Deepin_Vendor equals to "deepin".
    const auto deepinVendor = parseDBusField<QString>(appInfo, u8"X_Deepin_Vendor");
    item->displayName = getDisplayName(deepinVendor && deepinVendor.value() == QStringLiteral("deepin"),
                                       parseDBusField<QStringMap>(appInfo, u8"Name").value(),
                                       parseDBusField<QStringMap>(appInfo, u8"GenericName").value());

    if (auto value = parseDBusField<QStringMap>(appInfo, u8"Name")) {
        item->name = parseName(value.value());
    }

    if (auto value = parseDBusField<QStringMap>(appInfo, u8"Icons")) {
        item->iconName = parseIcon(value.value());
    }

    if (auto value = parseDBusField<qint64>(appInfo, u8"InstalledTime")) {
        item->installedTime = value.value();
    }

    if (auto value = parseDBusField<qint64>(appInfo, u8"LastLaunchedTime")) {
        item->lastLaunchedTime = value.value();
    }

    if (auto value = parseDBusField<bool>(appInfo, u8"AutoStart")) {
        item->isAutoStart = value.value();
    }

    return item;
}

AppMgr::AppMgr(QObject *parent)
    : QObject(parent)
    , m_objectManager(new AppManager1ApplicationObjectManager("org.desktopspec.ApplicationManager1",
                                                              "/org/desktopspec/ApplicationManager1",
                                                              QDBusConnection::sessionBus(), this))
{
    qDBusRegisterMetaType<ObjectInterfaceMap>();
    qDBusRegisterMetaType<ObjectMap>();
    qDBusRegisterMetaType<QStringMap>();
    qDBusRegisterMetaType<PropMap>();
    initObjectManager();
}

AppMgr::~AppMgr()
{
    for (auto item : std::as_const(m_appItems)) {
        if (auto handler = item->handler) {
            handler->deleteLater();
        }
    }
    qDeleteAll(m_appItems);
}

AppManager1Application * createAM1AppIfaceByPath(const QString &dbusPath)
{
    AppManager1Application * amAppIface = new AppManager1Application(QLatin1String("org.desktopspec.ApplicationManager1"),
                                                                    dbusPath,
                                                                    QDBusConnection::sessionBus());
    if (!amAppIface->isValid()) {
        qDebug() << "D-Bus interface not exist or failed to connect to" << dbusPath;
        return nullptr;
    }

    return amAppIface;
}

AppManager1Application * createAM1AppIface(const QString &desktopId)
{
    auto appItem = AppMgr::instance()->appItem(desktopId);
    if (!appItem) {
        qWarning() << "Can't find appItem for the desktopId" << desktopId;
        return nullptr;
    }
    qDebug() << "Get app interface for the desktopId" << desktopId;
    return appItem->handler;
}

// if return false, it means the launch is not even started.
// if return true, it means we attempted to launch it via AM, but not sure if it's succeed.
bool AppMgr::launchApp(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return false;

    const auto path = amAppIface->path();
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("dde-am", {"--by-user", path});
    if (!process.waitForFinished()) {
        qWarning() << "Failed to launch the desktopId:" << desktopId << process.errorString();
        return false;
    } else if (process.exitCode() != 0) {
        qWarning() << "Failed to launch the desktopId:" << desktopId << process.readAll();
        return false;
    }
    qDebug() << "Launch the desktopId" << desktopId;
    return true;
}

bool AppMgr::autoStart(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return false;

    return amAppIface->autoStart();
}

void AppMgr::setAutoStart(const QString &desktopId, bool autoStart)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return;

    amAppIface->setAutoStart(autoStart);
}

static const QStringList DisabledScaleEnvironments {
    "DEEPIN_WINE_SCALE=1",
    "QT_SCALE_FACTOR=1",
    "GDK_SCALE=1",
    "GDK_DPI_SCALE=1",
    "D_DXCB_DISABLE_OVERRIDE_HIDPI=1"
};

bool AppMgr::disableScale(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return 0;

    const auto environ = amAppIface->environ();
    const QStringList envs(environ.split(';'));
    // return true if envs contains any one of DisabledScaleEnvironments.
    auto iter = std::find_if(envs.begin(), envs.end(), [] (const QString &env) {
        return DisabledScaleEnvironments.contains(env);
    });
    return iter != envs.end();
}

void AppMgr::setDisableScale(const QString &desktopId, bool disableScale)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return;

    QString environ = amAppIface->environ();
    QStringList envs(environ.split(';', Qt::SkipEmptyParts));
    if (disableScale) {
        // remove all ScaleEnvironments, avoid other caller has set it manually.
        envs.removeIf([] (const QString &env) {
            auto iter = std::find_if(DisabledScaleEnvironments.begin(), DisabledScaleEnvironments.end(),
                                     [env] (const QString &item) {
                                         const auto left = item.split('=');
                                         const auto right = env.split('=');
                                         return !right.isEmpty() && left.at(0) == right.at(0);
                                     });
            return iter != DisabledScaleEnvironments.end();
        });
        envs << DisabledScaleEnvironments;
    } else {
        // remove all DisabledScaleEnvironments.
        envs.removeIf([] (const QString &env) {
            return DisabledScaleEnvironments.contains(env);
        });
    }

    environ = envs.join(';');
    qDebug() << "Update environ for the desktopId" << desktopId << ", env:" << environ;
    amAppIface->setEnviron(environ);
}

bool AppMgr::isOnDesktop(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return false;

    return amAppIface->isOnDesktop();
}

bool AppMgr::sendToDesktop(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return false;

    QDBusPendingReply<bool> reply = amAppIface->SendToDesktop();
    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error();
        return false;
    }

    return reply.value();
}

bool AppMgr::removeFromDesktop(const QString &desktopId)
{
    AppManager1Application * amAppIface = createAM1AppIface(desktopId);
    if (!amAppIface) return false;

    QDBusPendingReply<bool> reply = amAppIface->RemoveFromDesktop();
    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error();
        return false;
    }

    return reply.value();
}

bool AppMgr::isValid() const
{
    return m_objectManager->isValid();
}

QList<AppMgr::AppItem *> AppMgr::allAppInfosShouldBeShown() const
{
    return m_appItems.values();
}

AppMgr::AppItem *AppMgr::appItem(const QString &id) const
{
    const auto items = m_appItems.values();
    auto iter = std::find_if(items.begin(), items.end(), [id](AppItem *item) {
        return item->id == id;
    });
    return iter != items.end() ? *iter : nullptr;
}

void AppMgr::watchingAppItemPropertyChanged(const QString &key, AppMgr::AppItem *appItem)
{
    AppManager1Application * amAppIface = createAM1AppIfaceByPath(key);
    if (!amAppIface)
        return;

    Q_ASSERT(appItem->handler == nullptr);
    appItem->handler = amAppIface;
}

void AppMgr::updateAppsLaunchedTimes(const QVariantMap &appsLaunchedTimes)
{
    // need to update times for removed and updated.
    const auto &appItems = m_appItems.values();
    for (const auto item : std::as_const(appItems)) {
        auto iter = appsLaunchedTimes.find(item->appId);
        qint64 times = 0;
        if (iter != appsLaunchedTimes.cend())
            times = iter->toLongLong();

        // including reset and increase times.
        if (item->launchedTimes != times) {
            qDebug() << "LaunchedTimesChanged by DConfig, desktopId" << item->id;
            item->launchedTimes = times;
            Q_EMIT itemDataChanged(item->id);
        }
    }
}

void AppMgr::initObjectManager()
{
    if (!isValid())
        return;

    connect(m_objectManager, &AppManager1ApplicationObjectManager::InterfacesAdded, this,
            [this](const QDBusObjectPath &objPath, ObjectInterfaceMap interfacesAndProperties) {
                const QString key(objPath.path());
                qDebug() << "InterfacesAdded by AM, path:" << key;
                if (m_appItems.contains(objPath.path())) {
                    qWarning() << "App already exists for the path:" << key;
                    return;
                }
                if (auto appItem = parseDBus2AppItem(interfacesAndProperties)) {
                    qDebug() << "App item added, desktopId" << appItem->id;
                    watchingAppItemAdded(key, appItem);
                }
            });
    connect(m_objectManager, &AppManager1ApplicationObjectManager::InterfacesRemoved, this,
            [this](const QDBusObjectPath &objPath, const QStringList &interfaces) {
                Q_UNUSED(interfaces)
                const QString key(objPath.path());
                qDebug() << "InterfacesRemoved by AM, path:" << key;
                watchingAppItemRemoved(key);
            });

    fetchAppItems();

    DConfig *config = DConfig::create("org.deepin.dde.application-manager", "org.deepin.dde.am", "", this);
    if (!config->isValid()) {
        qWarning() << "DConfig is invalid when getting launched times.";
    } else {
        static const QString AppsLaunchedTimes(u8"appsLaunchedTimes");
        const auto &value = config->value(AppsLaunchedTimes).toMap();
        updateAppsLaunchedTimes(value);
        QObject::connect(config, &DConfig::valueChanged, this, [this, config](const QString &key) {
            if (key != AppsLaunchedTimes)
                return;

            qDebug() << "appsLaunchedTimes of DConfig Changed.";
            const auto &value = config->value(AppsLaunchedTimes).toMap();
            updateAppsLaunchedTimes(value);
        });
    }
}

void AppMgr::fetchAppItems()
{
    qDebug() << "Begin to fetch apps.";
    const auto reply = m_objectManager->GetManagedObjects();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *call){
        QDBusPendingReply<ObjectMap> reply = *call;
        if (reply.isError()) {
            qWarning() << "Failed to get apps from AM, " << reply.error();
            call->deleteLater();
            return;
        }
        qDebug() << "Fetched all AppItem, and start parsing data.";
        QMap<QString, AppMgr::AppItem *> items;
        const auto objects = reply.value();
        for (auto iter = objects.cbegin(); iter != objects.cend(); ++iter) {
            const auto &objPath = iter.key();
            const ObjectInterfaceMap &objs = iter.value();
            auto appItem = parseDBus2AppItem(objs);
            if (!appItem) {
                continue;
            }

            items[objPath.path()] = appItem;
            watchingAppItemPropertyChanged(objPath.path(), appItem);
        }
        call->deleteLater();
        qDebug() << "Fetched all AppItem, and end up parsing data.";

        m_appItems = items;
        Q_EMIT changed();
    });
    // TODO async to fetch apps.
    watcher->waitForFinished();
}

void AppMgr::watchingAppItemAdded(const QString &key, AppItem *appItem)
{
    m_appItems[key] = appItem;
    watchingAppItemPropertyChanged(key, appItem);
    Q_EMIT changed();
    Q_EMIT appItemAdd(key);
}

void AppMgr::watchingAppItemRemoved(const QString &key)
{
    auto appItem = m_appItems.value(key);
    if (!appItem)
        return;

    qDebug() << "App item removed, desktopId" << appItem->id;
    if (auto handler = appItem->handler) {
        handler->deleteLater();
    }
    m_appItems.remove(key);
    delete appItem;
    Q_EMIT changed();
    Q_EMIT appItemRemove(key);
}

AppMgr *AppMgr::instance() {
    static AppMgr *gInstance = nullptr;
    if (!gInstance) {
        gInstance = new AppMgr();
        gInstance->moveToThread(qApp->thread());
    }
    return gInstance;
}
