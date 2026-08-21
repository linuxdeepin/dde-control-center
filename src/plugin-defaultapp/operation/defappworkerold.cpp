// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "defappworkerold.h"
#include "defappmodel.h"
#include <QDBusPendingCall>
#include <QStringList>
#include <QList>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcDefaultWorkerOld, "dcc-default-worker-old")

DefAppWorkerOld::DefAppWorkerOld(DefAppModel *model, QObject *parent) :
    QObject(parent),
    m_defAppModel(model),
    m_dbusManager(new MimeDBusProxyOld(this))
{

    m_stringToCategory.insert("Browser",     Browser);
    m_stringToCategory.insert("Mail",        Mail);
    m_stringToCategory.insert("Text",        Text);
    m_stringToCategory.insert("Music",       Music);
    m_stringToCategory.insert("Video",       Video);
    m_stringToCategory.insert("Picture",     Picture);
    m_stringToCategory.insert("Terminal",    Terminal);

    connect(m_dbusManager, &MimeDBusProxyOld::Change, this, &DefAppWorkerOld::onGetListApps);

    m_userLocalPath = QDir::homePath() + "/.local/share/applications/";

    // mkdir folder
    QDir dir(m_userLocalPath);
    dir.mkpath(m_userLocalPath);
}

void DefAppWorkerOld::active()
{
    m_dbusManager->blockSignals(false);
}

void DefAppWorkerOld::deactive()
{
    m_dbusManager->blockSignals(true);
}

void DefAppWorkerOld::onSetDefaultApp(const QString &category, const App &item)
{
    QStringList mimelist = getTypeListByCategory(m_stringToCategory[category]);

    QDBusPendingCall call = m_dbusManager->SetDefaultApp(mimelist, item.Id);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [call, watcher, this, item, category] {
        if (!call.isError()) {
            qCDebug(DdcDefaultWorkerOld) << "Setting MIME " << category << "to " <<  item.Id;
            auto tosetCategory = getCategory(category);
            tosetCategory->setDefault(item);
        } else {
            qCWarning(DdcDefaultWorkerOld) << "Cannot set MIME" << category << "to" << item.Id;
        }
        watcher->deleteLater();
    });

}

void DefAppWorkerOld::onGetListApps()
{
    //遍历QMap去获取dbus数据
    for (auto  mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        const QString type { getTypeByCategory(mimelist.value()) };

        getDefaultAppFinished(mimelist.key(), m_dbusManager->GetDefaultApp(type));
        getListAppFinished(mimelist.key(),m_dbusManager->ListApps(type), false);
        getListAppFinished(mimelist.key(),m_dbusManager->ListUserApps(type), true);
    }
}

void DefAppWorkerOld::onDelUserApp(const QString &mime, const App &item)
{
    Category *category = getCategory(mime);

    category->delUserItem(item);
    if (item.CanDelete) {
        QStringList mimelist = getTypeListByCategory(m_stringToCategory[mime]);
        m_dbusManager->DeleteApp(mimelist, item.Id);
    } else {
        m_dbusManager->DeleteUserApp(item.Id);
    }

    //remove file
    QFile file(m_userLocalPath + item.Id);
    file.remove();
}

void DefAppWorkerOld::onCreateFile(const QString &mime, const QFileInfo &info)
{
    const bool isDesktop = info.suffix() == "desktop";

    if (isDesktop) {
        QFile file(info.filePath());
        QString newfile = m_userLocalPath + "deepin-custom-" + info.fileName();
        file.copy(newfile);
        file.close();

        QStringList mimelist = getTypeListByCategory(m_stringToCategory[mime]);
        QFileInfo fileInfo(info.filePath());

        const QString &filename = "deepin-custom-" + fileInfo.completeBaseName() + ".desktop";

        m_dbusManager->AddUserApp(mimelist, filename);

        App app;
        app.Id = filename;
        app.Name = fileInfo.baseName();
        app.DisplayName = fileInfo.baseName();
        app.Icon = "application-default-icon";
        app.Description = "";
        app.Exec = info.filePath();
        app.isUser = true;

        onGetListApps();
    } else {
        QFile file(m_userLocalPath + "deepin-custom-" + info.baseName() + ".desktop");

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }

        QTextStream out(&file);
        out << "[Desktop Entry]\n"
            "Type=Application\n"
            "Version=1.0\n"
            "Name=" + info.baseName() + "\n"
            "Path=" + info.path() + "\n"
            "Exec=" +  info.filePath() + "\n"
            "Icon=application-default-icon\n"
            "Terminal=false\n"
            "Categories=" + mime + ";"
#if (QT_VERSION < QT_VERSION_CHECK(5,15,0))
            << endl;
#else
            << Qt::endl;
#endif
        out.flush();
        file.close();

        QStringList mimelist = getTypeListByCategory(m_stringToCategory[mime]);

        QFileInfo fileInfo(info.filePath());
        m_dbusManager->AddUserApp(mimelist, "deepin-custom-" + fileInfo.baseName() + ".desktop");

        App app;
        app.Id = "deepin-custom-" + fileInfo.baseName() + ".desktop";
        app.Name = fileInfo.baseName();
        app.DisplayName = fileInfo.baseName();
        app.Icon = "application-default-icon";
        app.Description = "";
        app.Exec = info.filePath();
        app.isUser = true;

        onGetListApps();
    }
}

void DefAppWorkerOld::getListAppFinished(const QString &mime, const QString &defaultApp, bool isUser)
{
    const  QJsonArray defApp = QJsonDocument::fromJson(defaultApp.toUtf8()).array();
    saveListApp(mime, defApp, isUser);
}


void DefAppWorkerOld::getDefaultAppFinished(const QString &mime, const QString &w)
{
    const QJsonObject &defaultApp = QJsonDocument::fromJson(w.toStdString().c_str()).object();
    saveDefaultApp(mime, defaultApp);
}

void DefAppWorkerOld::saveListApp(const QString &mime, const QJsonArray &json, const bool isUser)
{
    Category *category = getCategory(mime);
    if (!category) {
        return;
    }

    QList<App> list;

    for (const QJsonValue &value : json) {
        QJsonObject obj = value.toObject();
        App app;
        app.Id = obj["Id"].toString();
        app.Name = obj["Name"].toString();
        app.DisplayName = obj["DisplayName"].toString();
        app.Icon = obj["Icon"].toString();
        app.Description = obj["Description"].toString();
        app.Exec = obj["Exec"].toString();
        app.isUser = isUser;
        app.CanDelete = obj["CanDelete"].toBool();
        app.MimeTypeFit = obj["MimeTypeFit"].toBool();

        list << app;
    }

    QList<App> systemList = category->systemAppList();
    QList<App> userList = category->userAppList();
    for (App app : list) {
        if (app.isUser == false) {
            for (App appUser : userList) {
                if (appUser.Exec == app.Exec) {
                    category->delUserItem(appUser);
                }
            }
        }
    }

    for (App app : list) {
        if (!systemList.contains(app) || !userList.contains(app)) {
            category->addUserItem(app);
        }
    }

    if (isUser) {
        userList = category->userAppList();
        for (App app : userList) {
            if (!list.contains(app)) {
                category->delUserItem(app);
            }
        }
    } else {
        systemList = category->systemAppList();
        for (App app : systemList) {
            if (!list.contains(app)) {
                category->delUserItem(app);
            }
        }
    }

    category->setCategory(mime);
}

void DefAppWorkerOld::saveDefaultApp(const QString &mime, const QJsonObject &json)
{
    Category *category = getCategory(mime);
    if (!category) {
        return;
    }
    category->setCategory(mime);

    App app;
    app.Id = json["Id"].toString();
    app.Name = json["Name"].toString();
    app.DisplayName = json["DisplayName"].toString();
    app.Icon = json["Icon"].toString();
    app.Description = json["Description"].toString();
    app.Exec = json["Exec"].toString();
    app.isUser = false;

    category->setDefault(app);
}

Category *DefAppWorkerOld::getCategory(const QString &mime) const
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

const QString DefAppWorkerOld::getTypeByCategory(const DefaultAppsCategory &category)
{
    return getTypeListByCategory(category)[0];
}

const QStringList DefAppWorkerOld::getTypeListByCategory(const DefaultAppsCategory &category)
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
