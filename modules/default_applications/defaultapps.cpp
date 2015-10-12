#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QObject>
#include <QDBusPendingReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "moduleheader.h"

#include "defaultapps.h"
#include "dbus/dbusdefaultapps.h"

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dbuttonlist.h>
#include <libdui/dswitchbutton.h>

DUI_USE_NAMESPACE

DefaultApps::DefaultApps() :
    m_dbusDefaultApps(this),
    m_dbusDefaultMedia(this)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Default Applications"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_centralWidget = new QFrame;

    m_header = new ModuleHeader(tr("Default Applications"));

    m_autoPlaySwitch = new DSwitchButton;

    DHeaderLine *defaultApps = new DHeaderLine;
    defaultApps->setTitle(tr("Default Applications"));

    DHeaderLine *autoPlayApplications = new DHeaderLine;
    autoPlayApplications->setTitle(tr("AutoPlay"));
    autoPlayApplications->setContent(m_autoPlaySwitch);

    m_appGrp = new DExpandGroup(this);
    m_mediaGrp = new DExpandGroup(this);
    m_modBrowser = new DArrowLineExpand;
    m_modBrowser->setTitle(tr("Browser"));
    m_modMail = new DArrowLineExpand;
    m_modMail->setTitle(tr("Mail"));
    m_modText = new DArrowLineExpand;
    m_modText->setTitle(tr("Text"));
    m_modMusic = new DArrowLineExpand;
    m_modMusic->setTitle(tr("Music"));
    m_modVideo = new DArrowLineExpand;
    m_modVideo->setTitle(tr("Video"));
    m_modPicture = new DArrowLineExpand;
    m_modPicture->setTitle(tr("Picture"));
    m_modTerminal = new DArrowLineExpand;
    m_modTerminal->setTitle(tr("Terminal"));
    m_modCDAudio = new DArrowLineExpand;
    m_modCDAudio->setTitle(tr("CD Audio"));
    m_modDVDVideo = new DArrowLineExpand;
    m_modDVDVideo->setTitle(tr("DVD Video"));
    m_modMusicPlayer = new DArrowLineExpand;
    m_modMusicPlayer->setTitle(tr("Music Player"));
    m_modCamera = new DArrowLineExpand;
    m_modCamera->setTitle(tr("Camera"));
    m_modSoftware = new DArrowLineExpand;
    m_modSoftware->setTitle(tr("Software"));

    QTimer::singleShot(1000, this, SLOT(lazyLoad()));

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->addWidget(defaultApps);
    scrollLayout->addWidget(new DSeparatorHorizontal);
    scrollLayout->addWidget(m_modBrowser);
    scrollLayout->addWidget(m_modMail);
    scrollLayout->addWidget(m_modText);
    scrollLayout->addWidget(m_modMusic);
    scrollLayout->addWidget(m_modVideo);
    scrollLayout->addWidget(m_modPicture);
    scrollLayout->addWidget(m_modTerminal);
    scrollLayout->addWidget(autoPlayApplications);
    scrollLayout->addWidget(new DSeparatorHorizontal);
    scrollLayout->addWidget(m_modCDAudio);
    scrollLayout->addWidget(m_modDVDVideo);
    scrollLayout->addWidget(m_modMusicPlayer);
    scrollLayout->addWidget(m_modCamera);
    scrollLayout->addWidget(m_modSoftware);
    //scrollLayout->addItem(vSpacer);
    //scrollLayout->addWidget(new QWidget);
    scrollLayout->addStretch(1);
    scrollLayout->setSpacing(0);
    scrollLayout->setMargin(0);

    QWidget *scrollWidget = new QWidget;
    scrollWidget->setLayout(scrollLayout);
    scrollWidget->setFixedWidth(310);

/*
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_header);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(scrollWidget);

    m_centralWidget->setLayout(mainLayout);
    m_centralWidget->updateGeometry();
    m_centralWidget->update();

    connect(m_autoPlaySwitch, &DSwitchButton::checkedChanged, this, &DefaultApps::setMediaOptionVisible);
}

DefaultApps::~DefaultApps()
{    
    qDebug() << "~DefaultApps()";

    m_centralWidget->setParent(nullptr);
    m_centralWidget->deleteLater();
}

QFrame* DefaultApps::getContent()
{
    return m_centralWidget;
}

void DefaultApps::reset()
{
    m_dbusDefaultApps.Reset();
    m_dbusDefaultMedia.Reset();
}

DArrowLineExpand *DefaultApps::createDefaultAppsExpand(const DefaultApps::DefaultAppsCategory &category, DArrowLineExpand *defaultApps)
{
    DButtonList *list = new DButtonList;
    list->setItemHeight(30);
    list->setItemWidth(310);

    const QString mime = getTypeByCategory(category);
    bool isMedia = false;

    QString appListJson;
    QString defaultAppJson;

    switch (category)
    {
    case Browser:
    case Mail:
    case Text:
    case Music:
    case Video:
    case Picture:
    case Terminal:      appListJson = m_dbusDefaultApps.ListApps(mime);
                        defaultAppJson = m_dbusDefaultApps.GetDefaultApp(mime);         break;

    case CD_Audio:
    case DVD_Video:
    case MusicPlayer:
    case Camera:
    case Software:      isMedia = true;
                        appListJson = m_dbusDefaultMedia.ListApps(mime);
                        defaultAppJson = m_dbusDefaultMedia.GetDefaultApp(mime);        break;
    }

    //qDebug() << mime << appListJson << defaultAppJson;

    QJsonArray appList = QJsonDocument::fromJson(appListJson.toStdString().c_str()).array();
    QJsonObject defaultApp = QJsonDocument::fromJson(defaultAppJson.toStdString().c_str()).object();

    int selected = -1;
    for (int i = 0; i != appList.size(); ++i)
    {
        list->addButton(appList.at(i).toObject().take("Name").toString());

        //qDebug() << appList.at(i).toObject() << defaultApp;

        if (appList.at(i).toObject() == defaultApp)
            selected = i;
    }
    if (selected != -1)
        list->checkButtonByIndex(selected);

    connect(m_header, &ModuleHeader::resetButtonClicked, [=] () -> void {
        if (!appList.count())
            return;

        const QString desktop = appList.at(0).toObject().take("Id").toString();
        if (desktop == "ignore" ||
            desktop == "nautilus-autorun-software.desktop")
            ;// TODO: select nothing. waitting for DButtonList update.
        else
            list->checkButtonByIndex(0);
    });

    connect(list, &DButtonList::buttonCheckedIndexChanged, [=] (int index) -> void {
        const QStringList mimeList = getTypeListByCategory(category);
        const QString appName = appList.at(index).toObject().take("Id").toString();
        QThread *t = nullptr;
        if (isMedia)
            t = new SetDefAppsThread(&m_dbusDefaultApps, mime, appName, mimeList);
        else
            t = new SetDefMediaThread(&m_dbusDefaultMedia, mime, appName, mimeList);
        t->start();
        connect(t, &QThread::finished, t, &QThread::deleteLater);
    });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(list);
    layout->setMargin(0);
    layout->setSpacing(0);

    QWidget *appsList = new QWidget;

    appsList->setFixedHeight(30 * appList.count());
    appsList->setLayout(layout);

    defaultApps->setContent(appsList);
    defaultApps->setExpand(false);

    return defaultApps;
}

const QString DefaultApps::getTypeByCategory(const DefaultApps::DefaultAppsCategory &category)
{
    switch (category)
    {
    case Browser:       return "x-scheme-handler/http";
    case Mail:          return "x-scheme-handler/mailto";
    case Text:          return "text/plain";
    case Music:         return "audio/mpeg";
    case Video:         return "video/mp4";
    case Picture:       return "image/jpeg";
    case Terminal:      return "application/x-terminal";
    case CD_Audio:      return "x-content/audio-cdda";
    case DVD_Video:     return "x-content/video-dvd";
    case MusicPlayer:   return "x-content/audio-player";
    case Camera:        return "x-content/image-dcf";
    case Software:      return "x-content/unix-software";
    }

    return QString();
}

const QStringList DefaultApps::getTypeListByCategory(const DefaultApps::DefaultAppsCategory &category)
{

    switch (category)
    {
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

void DefaultApps::setMediaOptionVisible(const bool visible)
{
    m_modCDAudio->setVisible(visible);
    m_modDVDVideo->setVisible(visible);
    m_modMusicPlayer->setVisible(visible);
    m_modCamera->setVisible(visible);
    m_modSoftware->setVisible(visible);

    m_dbusDefaultMedia.EnableAutoOpen(visible);
}

void DefaultApps::lazyLoad()
{
    const bool isMediaOpen = m_dbusDefaultMedia.autoOpen();
    m_autoPlaySwitch->setChecked(isMediaOpen);
    setMediaOptionVisible(isMediaOpen);

    m_modBrowser = createDefaultAppsExpand(Browser, m_modBrowser);
    m_modMail = createDefaultAppsExpand(Mail, m_modMail);
    m_modText = createDefaultAppsExpand(Text, m_modText);
    m_modMusic = createDefaultAppsExpand(Music, m_modMusic);
    m_modVideo = createDefaultAppsExpand(Video, m_modVideo);
    m_modPicture = createDefaultAppsExpand(Picture, m_modPicture);
    m_modTerminal = createDefaultAppsExpand(Terminal, m_modTerminal);

    m_appGrp->addExpand(m_modBrowser);
    m_appGrp->addExpand(m_modMail);
    m_appGrp->addExpand(m_modText);
    m_appGrp->addExpand(m_modMusic);
    m_appGrp->addExpand(m_modVideo);
    m_appGrp->addExpand(m_modPicture);
    m_appGrp->addExpand(m_modTerminal);

    m_modCDAudio = createDefaultAppsExpand(CD_Audio, m_modCDAudio);
    m_modDVDVideo = createDefaultAppsExpand(DVD_Video, m_modDVDVideo);
    m_modMusicPlayer = createDefaultAppsExpand(MusicPlayer, m_modMusicPlayer);
    m_modCamera = createDefaultAppsExpand(Camera, m_modCamera);
    m_modSoftware = createDefaultAppsExpand(Software, m_modSoftware);

    m_mediaGrp->addExpand(m_modCDAudio);
    m_mediaGrp->addExpand(m_modDVDVideo);
    m_mediaGrp->addExpand(m_modMusicPlayer);
    m_mediaGrp->addExpand(m_modCamera);
    m_mediaGrp->addExpand(m_modSoftware);
}

SetDefAppsThread::SetDefAppsThread(DBusDefaultApps *dbus, const QString &mime, const QString &appName, const QStringList &list) :
    dbus(dbus),
    mime(mime),
    appName(appName),
    list(list)
{
}

void SetDefAppsThread::run()
{
    for (const QString &mime : list)
        dbus->SetDefaultApp(mime, appName).waitForFinished();
}

SetDefMediaThread::SetDefMediaThread(DBusDefaultMedia *dbus, const QString &mime, const QString &appName, const QStringList &list) :
    dbus(dbus),
    mime(mime),
    appName(appName),
    list(list)
{
}

void SetDefMediaThread::run()
{
    for (const QString &mime : list)
        dbus->SetDefaultApp(mime, appName).waitForFinished();
}
