// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defappworker.h"

#include "defappmodel.h"

#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QLoggingCategory>
#include <QStringList>

Q_LOGGING_CATEGORY(DdcDefaultWorker, "dcc-default-worker")

const QString TerminalGSetting = QStringLiteral("com.deepin.desktop.default-applications.terminal");

DefAppWorker::DefAppWorker(DefAppModel *model, QObject *parent)
    : QObject(parent)
    , m_defAppModel(model)
    , m_dbusManager(new MimeDBusProxy(this))
    // , m_defaultTerminal(new QGSettings(TerminalGSetting.toLocal8Bit()))
{
    m_stringToCategory.insert("Browser", Browser);
    m_stringToCategory.insert("Mail", Mail);
    m_stringToCategory.insert("Text", Text);
    m_stringToCategory.insert("Music", Music);
    m_stringToCategory.insert("Video", Video);
    m_stringToCategory.insert("Picture", Picture);
    m_stringToCategory.insert("Terminal", Terminal);

    connect(m_dbusManager, &MimeDBusProxy::Change, this, &DefAppWorker::onGetListApps);

    m_userLocalPath = QDir::homePath() + "/.local/share/applications/";

    // mkdir folder
    QDir dir(m_userLocalPath);
    dir.mkpath(m_userLocalPath);
}

DefAppWorker::~DefAppWorker()
{
    // m_defaultTerminal->deleteLater();
}

void DefAppWorker::active()
{
    m_dbusManager->blockSignals(false);
}

void DefAppWorker::deactive()
{
    m_dbusManager->blockSignals(true);
}

void DefAppWorker::onSetDefaultApp(const QString &category, const App &item)
{
    if (category == "Terminal") {
        onSetDefaultTerminal(item);
        return;
    }
    QStringList mimelist = getTypeListByCategory(m_stringToCategory[category]);

    for (auto mimeType : mimelist) {
        QDBusPendingCall call = m_dbusManager->SetDefaultApp(mimeType, item.Id);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher,
                &QDBusPendingCallWatcher::finished,
                this,
                [watcher, this, item, category] {
                    if (!watcher->isError()) {
                        qCDebug(DdcDefaultWorker)
                                << "Setting MIME " << category << "to " << item.Id;
                        auto tosetCategory = getCategory(category);
                        tosetCategory->setDefault(item);
                    } else {
                        qCWarning(DdcDefaultWorker)
                                << "Cannot set MIME" << category << "to" << item.Id;
                    }
                    watcher->deleteLater();
                });
    }
}

void DefAppWorker::onSetDefaultTerminal(const App &item)
{
    Category *defaultTerinmalCategory = getCategory("Terminal");

    // m_defaultTerminal->set("app-id", item.Id);
    // m_defaultTerminal->set(
    //         "exec",
    //         QString("gdbus call --session --dest org.desktopspec.ApplicationManager1 --object-path "
    //                 "%1 --method org.desktopspec.ApplicationManager1.Application.Launch "
    //                 "'' [] {}")
    //                 .arg(item.dbusPath));

    defaultTerinmalCategory->setDefault(item);
}

void DefAppWorker::onGetListApps()
{
    // 遍历QMap去获取dbus数据
    for (auto mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd();
         ++mimelist) {
        if (mimelist.key() == "Terminal") {
            QDBusPendingReply<ObjectMap> call = m_dbusManager->GetManagedObjects();
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
            connect(watcher,
                    &QDBusPendingCallWatcher::finished,
                    this,
                    &DefAppWorker::getManagerObjectFinished);
            continue;
        }

        const QString type{ getTypeByCategory(mimelist.value()) };

        QDBusPendingReply<ObjectMap> call = m_dbusManager->ListApps(type);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, mimelist, type, this] {
            if (watcher->isError()) {
                qCWarning(DdcDefaultWorker) << "Cannot get AppList";
                watcher->deleteLater();
                return;
            }
            QDBusPendingReply<ObjectMap> call = *watcher;
            getListAppFinished(mimelist.key(), call.value());

            QDBusPendingReply<QString, QDBusObjectPath> getDefaultAppCall =
                    m_dbusManager->GetDefaultApp(type);
            QDBusPendingCallWatcher *defappWatcher =
                    new QDBusPendingCallWatcher(getDefaultAppCall, this);
            connect(defappWatcher,
                    &QDBusPendingCallWatcher::finished,
                    this,
                    [getDefaultAppCall, this, mimelist, type, defappWatcher] {
                        if (getDefaultAppCall.isError()) {
                            qCWarning(DdcDefaultWorker) << "Cannot get DefaultApp";
                            defappWatcher->deleteLater();
                            return;
                        }
                        QString mimeType = getDefaultAppCall.argumentAt<0>();
                        if (mimeType != type) {
                            qCWarning(DdcDefaultWorker) << "MimeType not match";
                            defappWatcher->deleteLater();
                            return;
                        }
                        QDBusObjectPath objectPath = getDefaultAppCall.argumentAt<1>();
                        if (objectPath.path() == "/") {
                            qCWarning(DdcDefaultWorker) << "Cannot find Mime: " << type;
                            defappWatcher->deleteLater();
                            return;
                        }
                        getDefaultAppFinished(mimelist.key(), m_dbusManager->getAppId(objectPath));
                        defappWatcher->deleteLater();
                    });
            watcher->deleteLater();
        });
    }
}

static QStringList getUILanguages()
{
    QLocale syslocal = QLocale::system();

    QStringList uiLanguages = syslocal.uiLanguages();
    // the nameMap uses underscore instead of minus sign
    for (auto &lang : uiLanguages) {
        lang.replace('-', '_');
    }
    uiLanguages << "default";

    return uiLanguages;
}

void DefAppWorker::getManagerObjectFinished(QDBusPendingCallWatcher *call)
{
    Category *category = getCategory("Terminal");
    QList<App> list;
    auto uiLanguages = getUILanguages();
    QDBusPendingReply<ObjectMap> reply = *call;
    if (reply.isError()) {
        call->deleteLater();
        return;
    }
    ObjectMap map = reply.value();
    for (auto it = map.cbegin(); it != map.cend(); it++) {
        QString dbusPath = it.key().path();
        auto mapInterface = it.value();
        for (const QVariantMap &mapInter : mapInterface) {
            if (mapInter.count() == 0) {
                continue;
            }
            if (mapInter.value("Categories").isNull()) {
                continue;
            }
            QStringList cats = qdbus_cast<QStringList>(mapInter["Categories"]);
            if (!cats.contains("TerminalEmulator")) {
                continue;
            }
            auto nameMap = qdbus_cast<QMap<QString, QString>>(mapInter["Name"]);
            auto genericNameMap = qdbus_cast<QMap<QString, QString>>(mapInter["GenericName"]);
            QString id = qdbus_cast<QString>(mapInter["ID"]);
            bool isDeepinVendor = qdbus_cast<QString>(mapInter["X_Deepin_Vendor"]) == "deepin";

            QString name = id;
            for (auto &lang : uiLanguages) {
                auto iter = nameMap.find(lang);
                if (iter != nameMap.end()) {
                    name = iter.value();
                    break;
                }
            }

            QString genericName;
            if (isDeepinVendor) {
                for (auto &lang : uiLanguages) {
                    auto iter = genericNameMap.find(lang);
                    if (iter != genericNameMap.end()) {
                        genericName = iter.value();
                        break;
                    }
                }
            }

            QString icon;
            if (auto mapicons = mapInter.value("Icons"); !mapicons.isNull()) {
                auto icons = qdbus_cast<QMap<QString, QString>>(mapicons);
                if (!icons.value("Desktop Entry").isNull()) {
                    icon = icons["Desktop Entry"];
                }
            }

            App app;
            app.dbusPath = dbusPath;
            app.Id = id;
            app.Name = name;
            app.DisplayName = genericName != "" ? genericName : name;
            app.Icon = icon;
            app.isUser = false;
            list << app;
            break;
        }
    }

    QList<App> systemList = category->getappItem();

    for (App app : list) {
        if (!systemList.contains(app)) {
            category->addUserItem(app);
        }
    }

    systemList = category->getappItem();
    for (App app : systemList) {
        if (!list.contains(app)) {
            category->delUserItem(app);
        }
    }
    category->setCategory("Terminal");

    // QString id = m_defaultTerminal->get("app-id").toString();
    // auto it = std::find_if(list.cbegin(), list.cend(), [id](const App &app) {
    //     return app.Id == id;
    // });

    // if (it != list.cend()) {
    //     category->setDefault(*it);
    // }

    call->deleteLater();
}

void DefAppWorker::onDelUserApp([[maybe_unused]] const QString &mime,
                                [[maybe_unused]] const App &item)
{
    // TODO: later
}

void DefAppWorker::onCreateFile([[maybe_unused]] const QString &mime,
                                [[maybe_unused]] const QFileInfo &info)
{
    // TODO: later
}

void DefAppWorker::getListAppFinished(const QString &mimeKey, const ObjectMap &map)
{
    Category *category = getCategory(mimeKey);
    if (!category) {
        return;
    }

    QList<App> list;
    auto uiLanguages = getUILanguages();

    for (auto it = map.cbegin(); it != map.cend(); it++) {
        QString dbusPath = it.key().path();
        auto mapInterface = it.value();
        for (const QVariantMap &mapInter : mapInterface) {
            if (mapInter.count() == 0) {
                continue;
            }

            if (auto nodisplay = mapInter.value("NoDisplay"); !nodisplay.isNull()) {
                if (qdbus_cast<bool>(nodisplay)) {
                    continue;
                }
            }

            auto nameMap = qdbus_cast<QMap<QString, QString>>(mapInter["Name"]);
            auto genericNameMap = qdbus_cast<QMap<QString, QString>>(mapInter["GenericName"]);
            QString id = qdbus_cast<QString>(mapInter["ID"]);
            bool isDeepinVendor = qdbus_cast<QString>(mapInter["X_Deepin_Vendor"]) == "deepin";

            QString name = id;
            for (auto &lang : uiLanguages) {
                auto iter = nameMap.find(lang);
                if (iter != nameMap.end()) {
                    name = iter.value();
                    break;
                }
            }

            QString genericName;
            if (isDeepinVendor) {
                for (auto &lang : uiLanguages) {
                    auto iter = genericNameMap.find(lang);
                    if (iter != genericNameMap.end()) {
                        genericName = iter.value();
                        break;
                    }
                }
            }

            QString icon;
            if (auto mapicons = mapInter.value("Icons"); !mapicons.isNull()) {
                auto icons = qdbus_cast<QMap<QString, QString>>(mapicons);
                if (!icons.value("Desktop Entry").isNull()) {
                    icon = icons["Desktop Entry"];
                }
            }

            App app;
            app.dbusPath = dbusPath;
            app.Id = id;
            app.Name = name;
            app.DisplayName = genericName != "" ? genericName : name;
            app.Icon = icon;
            app.isUser = false;
            list << app;
            break;
        }
    }

    QList<App> systemList = category->getappItem();

    for (App app : list) {
        if (!systemList.contains(app)) {
            category->addUserItem(app);
        }
    }

    systemList = category->getappItem();
    for (App app : systemList) {
        if (!list.contains(app)) {
            category->delUserItem(app);
        }
    }

    category->setCategory(mimeKey);
}

void DefAppWorker::getDefaultAppFinished(const QString &mimeKey, const QString &id)
{
    Category *category = getCategory(mimeKey);
    if (!category) {
        return;
    }

    auto items = category->getappItem();
    auto it = std::find_if(items.cbegin(), items.cend(), [id](const App &app) {
        return app.Id == id;
    });

    if (it != items.cend()) {
        category->setDefault(*it);
        category->setCategory(mimeKey);
    }
}

Category *DefAppWorker::getCategory(const QString &mime) const
{
    switch (m_stringToCategory[mime]) {
    case Browser:
        return m_defAppModel->getModBrowser();
    case Mail:
        return m_defAppModel->getModMail();
    case Text:
        return m_defAppModel->getModText();
    case Music:
        return m_defAppModel->getModMusic();
    case Video:
        return m_defAppModel->getModVideo();
    case Picture:
        return m_defAppModel->getModPicture();
    case Terminal:
        return m_defAppModel->getModTerminal();
    }
    return nullptr;
}

const QString DefAppWorker::getTypeByCategory(const DefaultAppsCategory &category)
{
    return getTypeListByCategory(category)[0];
}

// clang-format off
const QStringList DefAppWorker::getTypeListByCategory(const DefaultAppsCategory &category)
{
    switch (category) {
    case Browser:       return QStringList() << "x-scheme-handler/http" << "x-scheme-handler/ftp" << "x-scheme-handler/https"
                                   << "text/html" << "text/xml" << "text/xhtml_xml" << "text/xhtml+xml";
    case Mail:          return QStringList() << "x-scheme-handler/mailto" << "message/rfc822" << "application/x-extension-eml"
                                   << "application/x-xpinstall";
    case Text:          return QStringList() << "text/plain";
    case Music:         return QStringList() << "audio/mpeg" << "audio/mp3" << "audio/x-mp3" << "audio/mpeg3" << "audio/x-mpeg-3"
                                   << "audio/x-mpeg" << "audio/flac" << "audio/x-flac" << "application/x-flac"
                                   << "audio/ape" << "audio/x-ape" << "application/x-ape" << "audio/ogg" << "audio/x-ogg"
                                   << "audio/musepack" << "application/musepack" << "audio/x-musepack"
                                   << "application/x-musepack" << "audio/mpc" << "audio/x-mpc" << "audio/vorbis"
                                   << "audio/x-vorbis" << "audio/x-wav" << "audio/x-ms-wma";
    case Video:         return QStringList() << "video/mp4" << "audio/mp4" << "audio/x-matroska" << "video/x-matroska"
                                   << "application/x-matroska" << "video/avi" << "video/msvideo" << "video/x-msvideo"
                                   << "video/ogg" << "application/ogg" << "application/x-ogg" << "video/3gpp" << "video/3gpp2"
                                   << "video/flv" << "video/x-flv" << "video/x-flic" << "video/mpeg" << "video/x-mpeg"
                                   << "video/x-ogm" << "application/x-shockwave-flash" << "video/x-theora" << "video/quicktime"
                                   << "video/x-ms-asf" << "application/vnd.rn-realmedia" << "video/x-ms-wmv";
    case Picture:       return QStringList() << "image/jpeg" << "image/pjpeg" << "image/bmp" << "image/x-bmp" << "image/png"
                                   << "image/x-png" << "image/tiff" << "image/svg+xml" << "image/x-xbitmap" << "image/gif"
                                   << "image/x-xpixmap" << "image/vnd.microsoft.icon";
    case Terminal:      return QStringList() << "application/x-terminal";
    }
    return QStringList();
}

// clang-format on
