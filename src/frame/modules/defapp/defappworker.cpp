/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "defappworker.h"
#include "defappmodel.h"
#include "model/category.h"
#include "widgets/optionwidget.h"
#include <QStringList>
#include <QList>
#include <QFileInfo>
const QString ManagerService = "com.deepin.daemon.Mime";
using namespace dcc;
using namespace dcc::defapp;
DefAppWorker::DefAppWorker(DefAppModel *model, QObject *parent) :
    QObject(parent),
    m_defAppModel(model),
    m_dbusManager(new Mime(ManagerService, "/com/deepin/daemon/Mime", QDBusConnection::sessionBus(), this))
{

    m_dbusManager->setSync(false);

    m_stringToCategory.insert("Browser",     Browser);
    m_stringToCategory.insert("Mail",        Mail);
    m_stringToCategory.insert("Text",        Text);
    m_stringToCategory.insert("Music",       Music);
    m_stringToCategory.insert("Video",       Video);
    m_stringToCategory.insert("Picture",     Picture);
    m_stringToCategory.insert("Terminal",    Terminal);

    connect(m_dbusManager, &Mime::Change, this, &DefAppWorker::onGetListApps);

    m_userLocalPath = QDir::homePath()+ "/.local/share/applications/";

    // mkdir folder
    QDir dir(m_userLocalPath);
    dir.mkpath(m_userLocalPath);
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
    QStringList mimelist = getTypeListByCategory(m_stringToCategory[category]);
    m_dbusManager->SetDefaultApp(mimelist, item.Id);
    qDebug() << "setting MIME " << category << "to " <<  item.Id;
}

void DefAppWorker::onGetListApps()
{
    //遍历QMap去获取dbus数据
    for (auto  mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        const QString type { getTypeByCategory(mimelist.value()) };
        QDBusPendingCallWatcher *Default_Watcher = new QDBusPendingCallWatcher(m_dbusManager->GetDefaultApp(type), this);
        Default_Watcher->setProperty("mime", mimelist.key());
        connect(Default_Watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getDefaultAppFinished);

        QDBusPendingCallWatcher *System_Watcher = new QDBusPendingCallWatcher(m_dbusManager->ListApps(type), this);
        System_Watcher->setProperty("mime", mimelist.key());
        System_Watcher->setProperty("isUser", false);
        connect(System_Watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getListAppFinished);

        QDBusPendingCallWatcher *User_Watcher = new QDBusPendingCallWatcher(m_dbusManager->ListUserApps(type), this);
        User_Watcher->setProperty("mime", mimelist.key());
        User_Watcher->setProperty("isUser", true);
        connect(User_Watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getListAppFinished);
    }
}

void DefAppWorker::onResetTriggered()
{
    m_dbusManager->Reset();
}

void DefAppWorker::onDelUserApp(const QString &mime, const App &item)
{
    Category *category = getCategory(mime);

    category->delUserItem(item);
    m_dbusManager->DeleteUserApp(item.Id);

    //remove file
    QFile file(m_userLocalPath + item.Id);
    file.remove();
}

void DefAppWorker::onCreateFile(const QString &mime, const QFileInfo &info)
{
    const bool isDesktop = info.suffix() == "desktop";

    if (isDesktop) {
        QFile file(info.filePath());
        QString newfile = m_userLocalPath + "deepin-custom-"+ info.fileName();
        file.copy(newfile);
        file.close();

        QStringList mimelist = getTypeListByCategory(m_stringToCategory[mime]);
        QFileInfo fileInfo(info.filePath());

        const QString &filename = "deepin-custom-" + fileInfo.baseName() + ".desktop";

        m_dbusManager->AddUserApp(mimelist, filename);

        App app;
        app.Id = filename;
        app.Name = fileInfo.baseName();
        app.DisplayName = fileInfo.baseName();
        app.Icon = "application-default-icon";
        app.Description = "";
        app.Exec = info.filePath();
        app.isUser = true;

        Category *category = getCategory(mime);
        category->addUserItem(app);
    } else {
        QFile file(m_userLocalPath +"deepin-custom-" + info.baseName() + ".desktop");

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
            << endl;
        out.flush();
        file.close();

        QStringList mimelist = getTypeListByCategory(m_stringToCategory[mime]);

        QFileInfo fileInfo(info.filePath());
        m_dbusManager->AddUserApp(mimelist, "deepin-custom-" + fileInfo.baseName() + ".desktop").waitForFinished();

        App app;
        app.Id = "deepin-custom-" + fileInfo.baseName() + ".desktop";
        app.Name = fileInfo.baseName();
        app.DisplayName = fileInfo.baseName();
        app.Icon = "application-default-icon";
        app.Description = "";
        app.Exec = info.filePath();
        app.isUser = true;

        Category *category = getCategory(mime);
        category->addUserItem(app);
    }
}

void DefAppWorker::getListAppFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    const QString mime = w->property("mime").toString();
    const bool isUser = w->property("isUser").toBool();
    const  QJsonArray defaultApp = QJsonDocument::fromJson(reply.value().toUtf8()).array();
    saveListApp(mime, defaultApp, isUser);
    w->deleteLater();
}

void DefAppWorker::getDefaultAppFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    const QString mime = w->property("mime").toString();
    const QJsonObject &defaultApp = QJsonDocument::fromJson(reply.value().toStdString().c_str()).object();
    saveDefaultApp(mime, defaultApp);
    w->deleteLater();
}

void DefAppWorker::saveListApp(const QString &mime, const QJsonArray &json, const bool isUser)
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

        list << app;
    }

    QList<App> systemList = category->systemAppList();
    QList<App> userList = category->userAppList();

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
    }
    else {
        systemList = category->systemAppList();
        for (App app : systemList) {
            if (!list.contains(app)) {
                category->delUserItem(app);
            }
        }
    }

    category->setCategory(mime);
}

void DefAppWorker::saveDefaultApp(const QString &mime, const QJsonObject &json)
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
                                   << "image/x-xpixmap";
    case Terminal:      return QStringList() << "application/x-terminal";
    }
    return QStringList();
}
