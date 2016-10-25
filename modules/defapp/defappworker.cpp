#include "defappworker.h"

const QString ManagerService = "com.deepin.api.Mime";

DefAppWorker::DefAppWorker(DefAppModel *m_defAppModel, QObject *parent):
    m_defAppModel(m_defAppModel),
    m_dbusManager(new Manager(ManagerService, "/com/deepin/api/Manager", QDBusConnection::sessionBus(), this))
{
    Q_UNUSED(parent);
    m_stringToCategory.insert("Browser",  Browser);
    m_stringToCategory.insert("Mail",     Mail);
    m_stringToCategory.insert("Text",     Text);
    m_stringToCategory.insert("Music",    Music);
    m_stringToCategory.insert("Video",    Video);
    m_stringToCategory.insert("Picture",  Picture);
    m_stringToCategory.insert("Terminal", Terminal);

    m_dbusManager->setUseCache(true);
    m_dbusManager->setSync(false);
    connect(m_dbusManager, &Manager::Change, this, &DefAppWorker::onGetListAppsChanged);
    connect(m_dbusManager, &Manager::Change, this, &DefAppWorker::onGetDefaultAppChanged);
    onGetListAppsChanged();
    onGetDefaultAppChanged();
}

void DefAppWorker::onSetDefaultAppChanged(QString name,QString category) {
    QMap<QString, DefAppWorker::DefaultAppsCategory>::const_iterator i;
    for (i = m_stringToCategory.constBegin(); i != m_stringToCategory.constEnd(); ++i) {
        if(i.key() == category) {
            QStringList mimelist = getTypeListByCategory(i.value());
            for (const QString &mime : mimelist) {
                m_dbusManager->SetDefaultApp(mime, name).waitForFinished();
                qDebug()<< "已经设置" << name << mime;
            }
        }
    }
}

void DefAppWorker::onGetDefaultAppChanged() {
    //得到默认程序
    for (QMap<QString, DefAppWorker::DefaultAppsCategory>::const_iterator mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        QDBusPendingReply<QString> rep = m_dbusManager->GetDefaultApp(getTypeByCategory(mimelist.value()));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(rep, this);
        QDBusPendingReply<QString> reple = *watcher;
        watcher->deleteLater();
        const QJsonObject &defaultApp = QJsonDocument::fromJson(reple.value().toStdString().c_str()).object();
        const QString &defAppId = defaultApp.value("Id").toString();
        m_defapp.insertMulti(mimelist.key(), defAppId);
    }
    m_defAppModel->setDefApp(m_defapp);
}

void DefAppWorker::onGetListAppsChanged() {
    //遍历QMap去获取dbus数据
    QStringList list;
    for (QMap<QString, DefAppWorker::DefaultAppsCategory>::const_iterator mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        QDBusPendingReply<QString> rep = m_dbusManager->ListApps(getTypeByCategory(mimelist.value()));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(rep, this);
        QDBusPendingReply<QString> reple = *watcher;
        watcher->deleteLater();
        const  QJsonArray &defaultApp = QJsonDocument::fromJson(reple.value().toUtf8()).array();
        for (int i = 0; i != defaultApp.size(); ++i) {
            const QString &defAppName = defaultApp.at(i).toObject().take("Name").toString();
            const QString &defAppId = defaultApp.at(i).toObject().take("Id").toString();
            list.clear();
            list.append(defAppName);
            list.append(defAppId);
            list.append(mimelist.key());
            m_appsList.insertMulti(mimelist.key(), list);
        }
    }
    m_defAppModel->setList(m_appsList);
}

void DefAppWorker::onResetChanged() {
    m_dbusManager->Reset();
}

const QString DefAppWorker::getTypeByCategory(const DefaultAppsCategory &category){
    return getTypeListByCategory(category)[0];
}

const QStringList DefAppWorker::getTypeListByCategory(const DefaultAppsCategory &category) {
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
    case CD_Audio:      return QStringList() << "x-content/audio-cdda";
    case DVD_Video:     return QStringList() << "x-content/video-dvd";
    case MusicPlayer:   return QStringList() << "x-content/audio-player";
    case Camera:        return QStringList() << "x-content/image-dcf";
    case Software:      return QStringList() << "x-content/unix-software";
    }
    return QStringList();
}

