#include "defappworker.h"
#include "defappmodel.h"
#include "defapp/model/category.h"
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

    connect(m_dbusManager, &Mime::Change, this, &DefAppWorker::onGetDefaultApp);
    connect(m_dbusManager, &Mime::Change, this, &DefAppWorker::onGetListApps);

    m_userLocalPath = QDir::homePath()+ "/.local/share/applications/";
}

void DefAppWorker::active()
{
    m_dbusManager->blockSignals(false);
}

void DefAppWorker::deactive()
{
    m_dbusManager->blockSignals(true);
}

void DefAppWorker::onSetDefaultApp(const QString &category, const QJsonObject &item)
{
    QStringList mimelist = getTypeListByCategory(m_stringToCategory[category]);
    m_dbusManager->SetDefaultApp(mimelist, item["Id"].toString());
    qDebug() << "setting MIME " << category << "to " <<  item["Id"].toString();
}

void DefAppWorker::onGetDefaultApp()
{
    //得到默认程序
    for (QMap<QString, DefAppWorker::DefaultAppsCategory>::const_iterator mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        QDBusPendingReply<QString> rep = m_dbusManager->GetDefaultApp(getTypeByCategory(mimelist.value()));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(rep, this);
        watcher->setProperty("mime", mimelist.key());
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getDefaultAppFinished);

    }
}

void DefAppWorker::onGetListApps()
{
    //遍历QMap去获取dbus数据
    for (auto  mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        QDBusPendingReply<QString> rep = m_dbusManager->ListApps(getTypeByCategory(mimelist.value()));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(rep, this);
        watcher->setProperty("mime", mimelist.key());
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getListAppFinished);
    }
    //get user app list
    for (auto  mimelist = m_stringToCategory.constBegin(); mimelist != m_stringToCategory.constEnd(); ++mimelist) {
        QDBusPendingReply<QString> rep;
        rep = m_dbusManager->ListUserApps(getTypeByCategory(mimelist.value()));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(rep, this);
        watcher->setProperty("mime", mimelist.key());
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &DefAppWorker::getUserAppFinished);
    }
}

void DefAppWorker::onResetTriggered()
{
    m_dbusManager->Reset();
}

void DefAppWorker::onDelUserApp(const QString &mime, const QJsonObject &item)
{
    Category *category = getCategory(mime);
    category->delUserItem(item);
    m_dbusManager->DeleteUserApp(item["Id"].toString());

    //remove file
    QFile file(m_userLocalPath + item["Id"].toString());
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
        QJsonObject object;
        QFileInfo fileInfo(info.filePath());
        m_dbusManager->AddUserApp(mimelist, fileInfo.baseName() + ".desktop");
        object.insert("Id", "deepin-custom-" + fileInfo.baseName() + ".desktop");
        object.insert("Name", fileInfo.baseName());
        object.insert("DisplayName", fileInfo.baseName());
        object.insert("Icon", "application-default-icon");
        object.insert("Exec", info.filePath());
        Category *category = getCategory(mime);
        category->addUserItem(object);

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
        QJsonObject object;
        QFileInfo fileInfo(info.filePath());
        m_dbusManager->AddUserApp(mimelist, "deepin-custom-" + fileInfo.baseName() + ".desktop");
        object.insert("Id", "deepin-custom-" + fileInfo.baseName() + ".desktop");
        object.insert("Name", fileInfo.baseName());
        object.insert("DisplayName", fileInfo.baseName());
        object.insert("Icon", "application-default-icon");
        object.insert("Exec", info.filePath());
        Category *category = getCategory(mime);
        category->addUserItem(object);
    }
}

void DefAppWorker::getListAppFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    const QString mime = w->property("mime").toString();
    const  QJsonArray defaultApp = QJsonDocument::fromJson(reply.value().toUtf8()).array();
    saveListApp(mime, defaultApp);
    w->deleteLater();
}

void DefAppWorker::getUserAppFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;
    const QString mime = w->property("mime").toString();
    const  QJsonArray &defaultApp = QJsonDocument::fromJson(reply.value().toUtf8()).array();
    saveUserApp(mime, defaultApp);
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

void DefAppWorker::saveListApp(const QString &mime, const QJsonArray &json)
{
    Category *category = getCategory(mime);
    if (!category) {
        return;
    }
    QList<QJsonObject> t;
    for (int i = 0; i != json.size(); ++i) {
        const QJsonObject object = json.at(i).toObject();
        t.insert(t.end(), object);
    }
    category->setCategory(mime);
    category->setappList(t);
}

void DefAppWorker::saveUserApp(const QString &mime, const QJsonArray &json)
{
    qDebug() << mime;
    Category *category = getCategory(mime);
    if (!category) {
        return;
    }

    QList<QJsonObject> t;
    for (int i = 0; i != json.size(); ++i) {
        const QJsonObject object = json.at(i).toObject();
        t.insert(t.end(), object);
    }
    category->setCategory(mime);
    category->setuserList(t);
}

void DefAppWorker::saveDefaultApp(const QString &mime, const QJsonObject &app)
{
    Category *category = getCategory(mime);
    if (!category) {
        return;
    }
    category->setCategory(mime);
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

