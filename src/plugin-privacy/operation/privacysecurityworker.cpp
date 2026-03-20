// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "privacysecurityworker.h"
#include "privacysecuritydataproxy.h"
#include "applicationitem.h"
#include "dde-apps.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QTimer>
#include <QProcessEnvironment>
#include "QCoreApplication"
#include <QtConcurrent>

#include "applet.h"
#include <dsglobal.h>
#include "pluginloader.h"
#include "containment.h"
#include <appletbridge.h>

#include <polkit-qt6-1/PolkitQt1/Authority>
#include <qnamespace.h>

Q_DECLARE_LOGGING_CATEGORY(DCC_PRIVACY)
static const QString DESKTOP_ENTRY_ICON_KEY = "Desktop Entry";
static const QString DEEPIN_WINE_TEAM = "Deepin WINE Team";

const QString DataVersion = "1.0";
static QList<int> s_systemPrem = { ApplicationItem::CameraPermission };

static QString getDpkgArch()
{
    const static QString arch = []() {
        QString arch = QSysInfo::currentCpuArchitecture();
        QProcess pro;
        pro.start("/usr/bin/dpkg", QStringList() << "--print-architecture");
        pro.waitForFinished(2000);
        if (pro.exitCode() != 0) {
            qCWarning(DCC_PRIVACY) << "Failed to get dpkg architecture, dpkg error: " << pro.readAllStandardError().simplified() 
                << ", fallback to QSysInfo architecture:" << arch;
            return arch;
        }
        QString output = pro.readAllStandardOutput().simplified();
        if (output.isEmpty()) {
            qCWarning(DCC_PRIVACY) << "No architecture found for dpkg, fallback to QSysInfo architecture:" << arch;
            return arch;
        }
        return output;
    }();
    return arch;
}

PrivacySecurityWorker::PrivacySecurityWorker(PrivacySecurityModel *appsModel, QObject *parent)
    : QObject(parent)
    , m_model(appsModel)
    , m_dataProxy(new PrivacySecurityDataProxy(this))
{
    m_dpkgThreadPool = new QThreadPool(this);
    m_dpkgThreadPool->setMaxThreadCount(1);
    connect(m_dataProxy, &PrivacySecurityDataProxy::serviceExistsChanged, this, &PrivacySecurityWorker::serviceExistsChanged);
    init();
}

PrivacySecurityWorker::~PrivacySecurityWorker()
{
    m_dpkgThreadPool->waitForDone();

    if (!m_pathList.isEmpty())
        return;
    m_dataProxy->init();
}

void PrivacySecurityWorker::init()
{
    if (!m_pathList.isEmpty())
        return;
    m_dataProxy->init();
    // TODO：由于控制中心通过子线程加载插件后，会移动插件的加载线程->主线程，
    // 并删除原有线程->未指定父的对象所处的线程会被删除，所以使用qApp->主线程调用initApp
    QMetaObject::invokeMethod(qApp, [this]() {
        initApp();
    }, Qt::BlockingQueuedConnection);


    connect(m_dataProxy, &PrivacySecurityDataProxy::ModeChanged, this, &PrivacySecurityWorker::onModeChanged, Qt::QueuedConnection);
    connect(m_dataProxy, &PrivacySecurityDataProxy::EntityChanged, this, &PrivacySecurityWorker::onEntityChanged, Qt::QueuedConnection);
    connect(m_dataProxy, &PrivacySecurityDataProxy::PolicyChanged, this, &PrivacySecurityWorker::onPolicyChanged, Qt::QueuedConnection);

    QString envPath = QProcessEnvironment::systemEnvironment().value("PATH");
    m_pathList = envPath.split(':');

    m_dataProxy->listEntity();
    QStringList folders = { 
        "camera",
        m_model->premissiontoPath(ApplicationItem::DocumentFoldersPermission),
        m_model->premissiontoPath(ApplicationItem::PictureFoldersPermission),
        m_model->premissiontoPath(ApplicationItem::DesktopFoldersPermission),
        m_model->premissiontoPath(ApplicationItem::VideoFoldersPermission),
        m_model->premissiontoPath(ApplicationItem::MusicFoldersPermission),
        m_model->premissiontoPath(ApplicationItem::DownloadFoldersPermission)
    };
    for (const auto &folder : folders) {
        m_dataProxy->getMode(folder);
        m_dataProxy->getPolicy(folder);
    }
}

void PrivacySecurityWorker::initApp()
{
    auto rootApplet = qobject_cast<DS_NAMESPACE::DContainment *>(DS_NAMESPACE::DPluginLoader::instance()->rootApplet());
    auto applet = rootApplet->createApplet(DS_NAMESPACE::DAppletData{"org.deepin.ds.dde-apps"});
    applet->load();
    applet->init();

    DS_NAMESPACE::DAppletBridge bridge("org.deepin.ds.dde-apps");
    DS_NAMESPACE::DAppletProxy * amAppsProxy = bridge.applet();

    if (amAppsProxy) {
        QAbstractItemModel * model = amAppsProxy->property("appModel").value<QAbstractItemModel *>();
        m_ddeAmModel = model;

        connect(model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int last)
        {
            Q_UNUSED(parent)
            for (int i = first; i <= last; i++) {
                addAppItem(i);
            }
        });
        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last)
        {
            Q_UNUSED(parent)
            for (int i = last; i >= first; i--) {
                QString appId = m_ddeAmModel->data(m_ddeAmModel->index(i, 0), DS_NAMESPACE::AppItemModel::IdRole).toString();
                if (!appId.isEmpty()) {
                    m_model->removeApplictionItem(appId);
                }
            }
        }, Qt::DirectConnection);
    }
}

void PrivacySecurityWorker::setPremissionEnabled(int appItemIndex, int premission, bool enabled)
{
    auto item = m_model->appModel()->appList().value(appItemIndex);
    item->setPremissionEnabled(premission, enabled);
}
QString PrivacySecurityWorker::getAppPath(const QMap<QString, QString> &execs)
{
    static QStringList s_excludeBin = { "gio", "dbus-send", "python", "KboxAppLauncher", "/usr/bin/uengine-launch.sh" };
    QString appPath;
    for (auto it = execs.begin(); it != execs.end(); ++it) {
        if (it.key() != DESKTOP_ENTRY_ICON_KEY) {
            continue;
        }
        QString exec = it.value();
        if (s_excludeBin.contains(exec)) {
            continue;
        }

        QRegularExpression regex(R"(^\s*("|')([^"]+)\1|^(\S+))");
        QRegularExpressionMatch match = regex.match(exec);

        QString cleanedString;

        if (match.hasMatch()) {
            if (match.captured(2).isEmpty()) {
                cleanedString =  match.captured(3); // 没有引号的命令
            } else {
                cleanedString =  match.captured(2); // 引号包裹的命令
            }
        }

        if (QFile::exists(cleanedString)) {
            appPath = cleanedString;
            break;
        } else {
            for (const auto &binDir : m_pathList) {
                QDir bin(binDir);
                if (bin.exists(cleanedString)) {
                    appPath = bin.absolutePath() + "/" + cleanedString;
                    break;
                }
            }
        }
    }
    if (!appPath.isEmpty()) {
        while (true) {
            QFileInfo info(appPath);
            if (info.isSymLink()) {
                appPath = info.symLinkTarget();
            } else if (!info.exists()) {
                appPath.clear();
                break;
            } else {
                break;
            }
        }
    }
    return appPath;
}

void PrivacySecurityWorker::updateCheckAuthorizationing(bool checking)
{
    if (m_checkAuthorizationing != checking) {
        m_checkAuthorizationing = checking;
        if (m_checkAuthorizationing) {
            QTimer::singleShot(100, this, [this]() {
                if (m_checkAuthorizationing) {
                    Q_EMIT checkAuthorization(m_checkAuthorizationing);
                }
            });
        } else {
            Q_EMIT checkAuthorization(m_checkAuthorizationing);
        }
    }
}

// 刷新所有状态
void PrivacySecurityWorker::updateAllPermission()
{
    m_model->setBlackListByPackage(m_blacklistByPackage);
    for (auto &&it = m_blacklistByPackage.begin(); it != m_blacklistByPackage.end(); ++it) {
        QSet<QString> files;
        for (const auto &package : it.value()) {
            for (const auto &file : m_entityMap.value(package)) {
                files.insert(file);
            }
        }
        m_model->onAppPremissionEnabledChanged(it.key(), files);
    }
}

QString PrivacySecurityWorker::getEntityJson(const QString &name, bool isFile)
{
    QJsonObject obj;
    QJsonObject attrs;
    attrs.insert("bus_type", QString()); // 此处空项没有接口调用会报错
    attrs.insert("exes", QJsonArray());
    attrs.insert("interface", QString());
    attrs.insert("methods", QJsonArray());
    attrs.insert("object_path", QString());
    attrs.insert("service_name", QString());
    obj.insert("attrs", attrs);
    obj.insert("description", QString());
    obj.insert("description_zh_CN", QString());
    obj.insert("level", QString());
    obj.insert("owner", QString());
    obj.insert("sensitivity", QString());

    QJsonArray available_operations;
    available_operations.append("allow");
    available_operations.append("deny");
    obj.insert("name", name);
    obj.insert("available_operations", available_operations);
    obj.insert("subtype", isFile ? "file" : "");
    obj.insert("version", DataVersion);
    QJsonArray tags;
    tags.append("system");
    obj.insert("tags", tags);
    obj.insert("type", "object");
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QString PrivacySecurityWorker::getAppEntityJson(const ApplicationItem *item)
{
    QJsonObject obj;
    QJsonObject attrs;
    bool isPackage = !item->package().isEmpty();
    attrs.insert("bus_type", QString()); // 此处空项没有接口调用会报错
    attrs.insert("interface", QString());
    attrs.insert("methods", QJsonArray());
    attrs.insert("object_path", QString());
    attrs.insert("service_name", QString());
    obj.insert("description", item->name()); //
    obj.insert("description_zh_CN", item->name()); //
    obj.insert("level", QString());
    obj.insert("owner", QString());
    obj.insert("sensitivity", QString());

    if (isPackage) {
        QJsonArray exes = QJsonArray::fromStringList(item->executablePaths());
        attrs.insert("exes", exes);
    } else {
        QJsonArray exes = QJsonArray::fromStringList({item->appPath()});
        attrs.insert("exes", exes);
    }
    obj.insert("attrs", attrs);
    obj.insert("name", isPackage ? item->package() : item->appPath());
    QJsonArray available_operations;
    obj.insert("available_operations", available_operations);
    obj.insert("subtype", isPackage ? "package" : "file");
    obj.insert("version", DataVersion);
    QJsonArray tags;
    tags.append("system");
    obj.insert("tags", tags);
    obj.insert("type", "subject");
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QString PrivacySecurityWorker::getSubjectModeJson(const QString &name, bool isBlacklist)
{
    QJsonObject obj;
    obj.insert("mode", isBlacklist ? "blacklist" : "whitelist");
    obj.insert("object", name);
    obj.insert("version", DataVersion);
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QString PrivacySecurityWorker::getObjectPolicyJson(const ApplicationItem *item, int premission, bool enabled)
{
    QJsonObject obj;
    QJsonArray policies;
    QJsonObject policy;
    QJsonArray objects;

    QJsonObject object;
    object.insert("timestamp", 0);
    object.insert("valid_period", 0);
    object.insert("object", m_model->premissiontoPath(premission)); // 需要设置的路径
    object.insert("operations", enabled ? QJsonArray::fromStringList({ "allow" }) : QJsonArray::fromStringList({ "deny" }));
    objects.append(object);

    QJsonObject subject;
    subject.insert("name", item->package().isEmpty() ? item->appPath() : item->package());    // 包名
    policy.insert("objects", objects);
    policy.insert("subject", subject);
    policies.append(policy);
    obj.insert("policies", policies);
    obj.insert("version", DataVersion);
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

void PrivacySecurityWorker::updateAppPath(ApplicationItem *item)
{
    if (!item)
        return;

    m_pendingPathUpdates.append(item);

    if (!m_batchScheduled) {
        m_batchScheduled = true;
        QTimer::singleShot(0, this, &PrivacySecurityWorker::processBatchPathUpdates);
    }
}

void PrivacySecurityWorker::processBatchPathUpdates()
{
    m_batchScheduled = false;

    if (m_pendingPathUpdates.isEmpty())
        return;

    QList<ApplicationItem *> items;
    items.swap(m_pendingPathUpdates);

    struct ItemData {
        QString itemId;
        QString itemName;
        QMap<QString, QString> execs;
        QString desktopPath;
    };

    QList<ItemData> batch;
    batch.reserve(items.size());
    const auto &appList = m_model->appModel()->appList();
    for (auto *item : items) {
        if (!appList.contains(item))
            continue;
        ItemData d;
        d.itemId = item->id();
        d.itemName = item->name();
        d.execs = item->execs();
        d.desktopPath = item->desktopPath();
        batch.append(std::move(d));
    }

    if (batch.isEmpty())
        return;

    (void)QtConcurrent::run(m_dpkgThreadPool, [this, batch]() {
        struct ProcessedItem {
            QString itemId;
            QString itemName;
            QString appPath;
            QString dpkgQueryPath;
        };

        QList<ProcessedItem> processed;
        processed.reserve(batch.size());
        QStringList allDpkgPaths;

        for (const auto &d : batch) {
            ProcessedItem p;
            p.itemId = d.itemId;
            p.itemName = d.itemName;
            p.appPath = getAppPath(d.execs);
            p.dpkgQueryPath = d.desktopPath.isEmpty() ? p.appPath : d.desktopPath;
            processed.append(p);

            if (!p.dpkgQueryPath.isEmpty()) {
                allDpkgPaths.append(p.dpkgQueryPath);
            }
        }

        QMap<QString, QString> pathToPackage;
        if (!allDpkgPaths.isEmpty()) {
            pathToPackage = batchGetPackageNames(allDpkgPaths);
        }

        for (const auto &p : processed) {
            QString packageName = pathToPackage.value(p.dpkgQueryPath);
            QMetaObject::invokeMethod(this,
                [this, itemId = p.itemId, itemName = p.itemName,
                 path = p.appPath, packageName]() {
                    if (path.isEmpty()) {
                        qCInfo(DCC_PRIVACY) << "Exclude app id: " << itemId
                                            << ", name: " << itemName
                                            << "because it appPath is empty";
                        m_model->removeApplictionItem(itemId);
                        return;
                    }
                    const auto &appList = m_model->appModel()->appList();
                    auto it = std::find_if(appList.cbegin(), appList.cend(),
                        [&itemId](ApplicationItem *app) { return app->id() == itemId; });
                    if (it == appList.cend())
                        return;
                    ApplicationItem *item = *it;
                    item->onAppPathChanged(path);
                    item->onPackageChanged(packageName);
                    m_model->updatePermission(item);
                    m_model->emitAppDataChanged(item);
                },
                Qt::QueuedConnection);
        }
    });
}

QMap<QString, QString> PrivacySecurityWorker::batchGetPackageNames(const QStringList &paths)
{
    QMap<QString, QString> result;
    QSet<QString> uniqueSet(paths.begin(), paths.end());
    QStringList uniquePaths(uniqueSet.begin(), uniqueSet.end());

    const int chunkSize = 100;
    for (int i = 0; i < uniquePaths.size(); i += chunkSize) {
        QStringList chunk = uniquePaths.mid(i, chunkSize);

        QProcess pro;
        QStringList args;
        args << "-S";
        args.append(chunk);
        pro.start("/usr/bin/dpkg-query", args);
        if (!pro.waitForFinished(5000)) {
            qCWarning(DCC_PRIVACY) << "dpkg-query batch timed out for chunk starting at index" << i
                                   << ", killing process";
            pro.kill();
            pro.waitForFinished(1000);
            continue;
        }

        // dpkg-query returns exit code 1 when any path is not found,
        // but still prints found packages to stdout
        QString output = pro.readAllStandardOutput();
        const QStringList lines = output.split('\n', Qt::SkipEmptyParts);

        for (const QString &line : lines) {
            // Format: "package-name: /path/to/file"
            // Or:     "package-name:amd64: /path/to/file"
            int separatorIdx = line.indexOf(": /");
            if (separatorIdx < 0)
                continue;

            QString packagePart = line.left(separatorIdx);
            QString filePath = line.mid(separatorIdx + 2).trimmed();
            QString packageName = packagePart.split(':').first();

            if (!result.contains(filePath)) {
                result.insert(filePath, packageName);
            }
        }

        if (pro.exitCode() != 0) {
            qCDebug(DCC_PRIVACY) << "dpkg-query batch: some paths not found, stderr:"
                                 << pro.readAllStandardError().simplified();
        }
    }

    return result;
}

ApplicationItem *PrivacySecurityWorker::addAppItem(int dataIndex)
{
    // 不展示的应用
    static const QStringList s_excludeApp = {
        "dde-computer", "org.deepin.dde.control-center", "dde-file-manager", "dde-trash", "deepin-manual", "deepin-terminal",
        "onboard", "onboard-settings",
    };

    const auto &NoDisplay = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::NoDisplayRole).toBool();

    if (NoDisplay) {
        return nullptr;
    }

    const auto &name = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::NameRole).toString();
    const auto &iconName = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::IconNameRole).toString();
    const auto &isLingLong = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::XLingLongRole).toBool();
    const auto &XCreatedBy = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::XCreatedByRole).toString();
    const auto &id = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::IdRole).toString();
    const auto &execs = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::ExecsRole).value<QMap<QString, QString>>();
    const auto &desktopSoucePath = m_ddeAmModel->data(m_ddeAmModel->index(dataIndex, 0), DS_NAMESPACE::AppItemModel::DesktopSourcePathRole).toString();

    for (const auto &app : s_excludeApp) {
        if (id.contains(app)) {
            qCInfo(DCC_PRIVACY) << "Exclude app id: " << id << ", name: " << name << "because it is in exclude list";
            return nullptr;
        }
    }

    if (isLingLong) {
        qCInfo(DCC_PRIVACY) << "Exclude app id: " << id << ", name: " << name << "because it is a linglong application";
        return nullptr;
    }

    if (XCreatedBy == DEEPIN_WINE_TEAM) {
        qCInfo(DCC_PRIVACY) << "Exclude app id: " << id << ", name: " << name << "because it is a wine application";
        return nullptr;
    }

    ApplicationItem *appItem = new ApplicationItem();
    appItem->onIdChanged(id);
    appItem->onNameChanged(name);
    appItem->onIconChanged(iconName);
    appItem->onExecsChanged(execs);
    appItem->onDesktopPathChanged(desktopSoucePath);
    if (m_model->addApplictionItem(appItem)) {
        updateAppPath(appItem);
        connect(appItem, &ApplicationItem::requestSetPremissionEnabled, this, &PrivacySecurityWorker::setAppPermissionEnable);
    } else {
        delete appItem;
        appItem = nullptr;
    }
    return appItem;
}

void PrivacySecurityWorker::onModeChanged(const QString &mode, const QString &type)
{
    if (type != "add" && type != "modify")
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(mode.toLatin1(), &jsonError);
    if (doc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qCWarning(DCC_PRIVACY) << "mode changed :json parse error:" << jsonError.errorString();
        return;
    }
    QJsonObject obj = doc.object();
    if (obj.value("version").toString() != DataVersion) {
        qCWarning(DCC_PRIVACY) << "mode changed :version error: current version:" << DataVersion << "json version:" << obj.value("version").toString();
        return;
    }
    QString object = obj.value("object").toString();
    QString objMode = obj.value("mode").toString();
    int modeType = PrivacySecurityDataProxy::AllEnable;
    if (objMode == "blacklist") {
        modeType = PrivacySecurityDataProxy::ByCustom;
    } else if (objMode == "allallow") {
        modeType = PrivacySecurityDataProxy::AllEnable;
    }
    int premission = m_model->pathtoPremission(object, false);
    if (premission != 0)
        m_model->onPremissionModeChanged(premission, modeType);
}

void PrivacySecurityWorker::onEntityChanged(const QString &entity, const QString &type)
{
    if (type != "add" && type != "modify")
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(entity.toLatin1(), &jsonError);
    if (doc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qCWarning(DCC_PRIVACY) << "entity changed :json parse error:" << jsonError.errorString();
        return;
    }
    QJsonObject obj = doc.object();
    if (obj.value("version").toString() != DataVersion) {
        qCWarning(DCC_PRIVACY) << "entity changed :version error: current version:" << DataVersion << "json version:" << obj.value("version").toString();
        return;
    }
    QString name = obj.value("name").toString();
    QSet<QString> exes;
    if (name.isEmpty()) {
        qCWarning(DCC_PRIVACY) << "entity changed :name is empty";
        return;
    }
    QJsonObject attrs = obj.value("attrs").toObject();
    if (!attrs.isEmpty()) {
        QJsonArray exesJson = attrs.value("exes").toArray();
        for (const auto it : exesJson) {
            exes.insert(it.toString());
        }
    }
    m_entityMap.insert(name, exes);
    updateAllPermission();
}

void PrivacySecurityWorker::onPolicyChanged(const QString &policy, const QString &type)
{
    if (type != "add" && type != "modify")
        return;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(policy.toLatin1(), &jsonError);
    if (doc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qCWarning(DCC_PRIVACY) << "policy changed :json parse error:" << jsonError.errorString();
        return;
    }
    QJsonObject obj = doc.object();
    if (obj.value("version").toString() != DataVersion) {
        qCWarning(DCC_PRIVACY) << "policy changed :version error: current version:" << DataVersion << "json version:" << obj.value("version").toString();
        return;
    }
    QJsonArray policies = obj.value("policies").toArray();
    for (const auto policy : policies) {
        QJsonObject policyObj = policy.toObject();
        QString package = policyObj.value("subject").toObject().value("name").toString();
        QJsonArray objects = policyObj.value("objects").toArray();
        for (const auto object : objects) {
            QJsonObject objectObj = object.toObject();
            QString objectName = objectObj.value("object").toString();
            QVariantList operations = objectObj.value("operations").toArray().toVariantList();
            if (operations.contains("deny")) {
                m_blacklistByPackage[objectName].insert(package);
            } else {
                m_blacklistByPackage[objectName].remove(package);
            }
        }
    }
    updateAllPermission();
}

void PrivacySecurityWorker::setAppPermissionEnableByCheck(bool ok)
{
    while (!m_cacheAppPermission.isEmpty()) {
        const auto &&it = m_cacheAppPermission.takeFirst();
        ApplicationItem *item = it.first;
        int premission = it.second.first;
        bool enabled = it.second.second;
        if (ok) {
            QString file = m_model->premissiontoPath(premission);
            if (file.isEmpty()) {
                item->emitDataChanged();
                m_model->emitAppDataChanged(item);
                continue;
            }
            QStringList executablePaths = item->executablePaths();
            if (executablePaths.isEmpty()) {
                QString path = item->appPath();
                executablePaths = getExecutable(item->package());
                item->onExecutablePathsChanged(executablePaths);
            }

            // 检查是否有权限实体
            if (!m_entityMap.contains(file)) {  
                m_dataProxy->setEntity(getEntityJson(file, premission != ApplicationItem::CameraPermission));
            }
            // 检查有没有二进制或者包的实体，只有有实体之后才设置权限策略
            if ((!item->package().isEmpty() && !m_entityMap.contains(item->package()))
                || (item->package().isEmpty() && !m_entityMap.contains(item->appPath()))) {
                m_dataProxy->setEntity(getAppEntityJson(item));
            }
            m_dataProxy->setMode(getSubjectModeJson(file, true));   // 设置黑名单模式, 设置客体模式
            m_dataProxy->setPolicy(getObjectPolicyJson(item, premission, enabled));

        } else {
            item->emitDataChanged();
            m_model->emitAppDataChanged(item);
        }
    }
}

void PrivacySecurityWorker::setAppPermissionEnable(int premission, bool enabled, ApplicationItem *item)
{
    m_cacheAppPermission.append({ item, { premission, enabled } }); // 存入缓存
    if (!s_systemPrem.contains(premission)) { // 如果申请的权限不包括列表中的权限（比如相机以外的权限）
        setAppPermissionEnableByCheck(true);
        return;
    }

    if (m_checkAuthorizationing)
        return;

    connect(PolkitQt1::Authority::instance(), &PolkitQt1::Authority::checkAuthorizationFinished, this, [this](PolkitQt1::Authority::Result authenticationResult) {
        disconnect(PolkitQt1::Authority::instance(), nullptr, this, nullptr);
        updateCheckAuthorizationing(false);
        setAppPermissionEnableByCheck(PolkitQt1::Authority::Result::Yes == authenticationResult);
    });
    updateCheckAuthorizationing(true);
    PolkitQt1::Authority::instance()->checkAuthorization("com.deepin.FileArmor1", PolkitQt1::UnixProcessSubject(getpid()), PolkitQt1::Authority::AllowUserInteraction);
}

void PrivacySecurityWorker::checkAuthorizationCancel()
{
    if (m_checkAuthorizationing) {
        //        PolkitQt1::Authority::instance()->checkAuthorizationCancel(); // 取消后不能再拉起
        disconnect(PolkitQt1::Authority::instance(), nullptr, this, nullptr);
        updateCheckAuthorizationing(false);
    }
}

QStringList PrivacySecurityWorker::getExecutable(const QString &packageName)
{
    if (packageName.isEmpty()) {
        qCWarning(DCC_PRIVACY) << "getExecutable failed: packageName is empty";
        return {};
    }
    QString listFilePath = QString("/var/lib/dpkg/info/%1.list").arg(packageName);
    if (!QFile::exists(listFilePath)) {
        listFilePath = QString("/var/lib/dpkg/info/%1.list").arg(packageName + ":" + getDpkgArch());
    }
    QFile listFile(listFilePath);

    if (!listFile.exists() || !listFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(DCC_PRIVACY) << "Failed to open list file:" << listFilePath;
        return {};
    }

    QStringList files;
    QTextStream stream(&listFile);
    while (!stream.atEnd()) {
        QString filePath = stream.readLine().trimmed();
        if (filePath.isEmpty()) {
            continue;
        }

        QFileInfo fileInfo(filePath);
        if (fileInfo.exists() && 
            fileInfo.isFile() && 
            fileInfo.isExecutable() && 
            !QLibrary::isLibrary(filePath)) {
            files << filePath;
        }
    }

    listFile.close();
    return files;
}
