/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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

#include "helper.h"
#include "moduleheader.h"
#include "constants.h"
#include "defaultapps.h"
#include "runnabletask.h"

#include "dbus/dbusdefaultapps.h"

#include <dtextbutton.h>
#include <dseparatorhorizontal.h>
#include <dbaseline.h>
#include <darrowlineexpand.h>
#include <dboxwidget.h>
#include <doptionlist.h>

DWIDGET_USE_NAMESPACE

DefaultAppsModule::DefaultAppsModule()
{
}

QFrame *DefaultAppsModule::getContent()
{
    qDebug() << "new DefaultApps begin";
    static DefaultApps *frame = NULL;
    if (!frame) {
        frame = new DefaultApps;
    }
    qDebug() << "new DefaultApps end";
    return frame->getContent();
}

DBusDefaultMediaThread::DBusDefaultMediaThread(DBusDefaultMedia *dbus)
{
    m_dbus = dbus;
}

void DBusDefaultMediaThread::run()
{
    qDebug() << QThread::currentThread();
    m_dbus->autoOpen();
    qDebug() << qApp->thread();
    emit dbusConnected();
}

DefaultApps::DefaultApps() :
    m_dbusDefaultApps(this)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Default Applications"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_dbusDefaultMedia = new DBusDefaultMedia;
    DBusDefaultMediaThread *dbusConnetor = new DBusDefaultMediaThread(m_dbusDefaultMedia);
    QThread *dbusThread = new QThread;
    connect(dbusThread, &QThread::started, dbusConnetor, &DBusDefaultMediaThread::run);
    connect(dbusThread, &QThread::finished, dbusConnetor, &DBusDefaultMediaThread::deleteLater);
    connect(dbusThread, &QThread::finished, dbusThread, &QThread::deleteLater);
    dbusConnetor->moveToThread(dbusThread);

    m_centralWidget = new QFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_centralWidget->setLayout(mainLayout);

    m_header = new ModuleHeader(tr("Default Applications"));

    mainLayout->addWidget(m_header);
    mainLayout->addWidget(new DSeparatorHorizontal);

    QScrollArea *scrollArea = new QScrollArea;
    mainLayout->addWidget(scrollArea);

    scrollArea->setStyleSheet("background-color:transparent;");
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    DVBoxWidget *scrollContent = new DVBoxWidget;
    scrollArea->setWidget(scrollContent);

    scrollContent->layout()->setSpacing(0);
    scrollContent->layout()->setMargin(0);
    scrollContent->setFixedWidth(DCC::ModuleContentWidth);

    m_autoPlaySwitch = new DSwitchButton;
    DHeaderLine *defaultApps = new DHeaderLine;
    DHeaderLine *autoPlayApplications = new DHeaderLine;
    m_appGrp = new DExpandGroup(this);
    m_mediaGrp = new DExpandGroup(this);
    m_modBrowser = new DArrowLineExpand;
    m_modMail = new DArrowLineExpand;
    m_modText = new DArrowLineExpand;
    m_modMusic = new DArrowLineExpand;
    m_modVideo = new DArrowLineExpand;
    m_modPicture = new DArrowLineExpand;
    m_modTerminal = new DArrowLineExpand;
    m_modCDAudio = new DArrowLineExpand;
    m_modDVDVideo = new DArrowLineExpand;
    m_modMusicPlayer = new DArrowLineExpand;
    m_modCamera = new DArrowLineExpand;
    m_modSoftware = new DArrowLineExpand;

    autoPlayApplications->setTitle(tr("AutoPlay"));
    autoPlayApplications->setContent(m_autoPlaySwitch);
    defaultApps->setTitle(tr("Default Applications"));
    m_modSoftware->hide();
    m_modSoftware->setTitle(tr("Software"));
    m_modCamera->hide();
    m_modCamera->setTitle(tr("Camera"));
    m_modMusicPlayer->hide();
    m_modMusicPlayer->setTitle(tr("Music Player"));
    m_modDVDVideo->hide();
    m_modDVDVideo->setTitle(tr("DVD Video"));
    m_modCDAudio->hide();
    m_modCDAudio->setTitle(tr("CD Audio"));
    m_modTerminal->setTitle(tr("Terminal"));
    m_modPicture->setTitle(tr("Picture"));
    m_modVideo->setTitle(tr("Video"));
    m_modMusic->setTitle(tr("Music"));
    m_modText->setTitle(tr("Text"));
    m_modMail->setTitle(tr("Mail"));
    m_modBrowser->setTitle(tr("Browser"));

    scrollContent->layout()->addWidget(defaultApps);
    scrollContent->layout()->addWidget(new DSeparatorHorizontal);
    scrollContent->layout()->addWidget(m_modBrowser);
    scrollContent->layout()->addWidget(m_modMail);
    scrollContent->layout()->addWidget(m_modText);
    scrollContent->layout()->addWidget(m_modMusic);
    scrollContent->layout()->addWidget(m_modVideo);
    scrollContent->layout()->addWidget(m_modPicture);
    scrollContent->layout()->addWidget(m_modTerminal);
    scrollContent->layout()->addWidget(autoPlayApplications);
    scrollContent->layout()->addWidget(new DSeparatorHorizontal);
    scrollContent->layout()->addWidget(m_modCDAudio);
    scrollContent->layout()->addWidget(m_modDVDVideo);
    scrollContent->layout()->addWidget(m_modMusicPlayer);
    scrollContent->layout()->addWidget(m_modCamera);
    scrollContent->layout()->addWidget(m_modSoftware);
    scrollContent->layout()->addStretch(1);

    qDebug() << "begin update";
    m_centralWidget->updateGeometry();
    m_centralWidget->update();
    qDebug() << "end update";

    connect(dbusConnetor, &DBusDefaultMediaThread::dbusConnected,
            this, &DefaultApps::lazyLoad);
    dbusThread->start();

    connect(m_header, &ModuleHeader::resetButtonClicked, this, &DefaultApps::resetDefaults, Qt::QueuedConnection);
    connect(m_autoPlaySwitch, &DSwitchButton::checkedChanged, this, &DefaultApps::setMediaOptionVisible);
    connect(&m_dbusDefaultApps, &DBusDefaultApps::Change, this, &DefaultApps::updateListCheckedIndex);
}

DefaultApps::~DefaultApps()
{
    qDebug() << "~DefaultApps()";

    m_centralWidget->setParent(nullptr);
    m_centralWidget->deleteLater();
}

QFrame *DefaultApps::getContent()
{
    return m_centralWidget;
}


DArrowLineExpand *DefaultApps::createDefaultAppsExpand(const DefaultApps::DefaultAppsCategory &category, DArrowLineExpand *defaultApps)
{
    DOptionList *list = new DOptionList;
    list->setFixedWidth(DCC::ModuleContentWidth);
//    list->setItemHeight(30);
//    list->setItemWidth(310);

    const QString mime = getTypeByCategory(category);
    const bool isMedia = isMediaApps(category);

    QString appListJson;

    if (isMediaApps(category)) {
        appListJson = m_dbusDefaultMedia->ListApps(mime);
    } else {
        appListJson = m_dbusDefaultApps.ListApps(mime);
    }

    QJsonArray appList = QJsonDocument::fromJson(appListJson.toStdString().c_str()).array();

    QString appDisplayName;
    QString appName;
    QString id;
    QString icon;

    for (int i = 0; i != appList.size(); ++i) {
        appName = appList.at(i).toObject().take("Name").toString();
        appDisplayName = appList.at(i).toObject().take("DisplayName").toString();
        id = appList.at(i).toObject().take("Id").toString();
        icon = appList.at(i).toObject().take("Icon").toString();

        DOption *option = new DOption;
        option->setName(appDisplayName.isEmpty() ? appName : appDisplayName);
        option->setValue(id);
        option->setFixedHeight(30);

//        if (!isMedia)
        {
            QPixmap pixmap(Helper::searchAppIcon(QStringList() << icon << "application-x-desktop", 16));
            option->setIcon(pixmap.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        list->addOption(option);
    }
    m_appsBtnList[category] = list;

    connect(list, &DOptionList::currentRowChanged, [ = ](int index) {
        const QStringList mimeList = getTypeListByCategory(category);
        const QString appName = appList.at(index).toObject().take("Id").toString();
        QThread *t = new QThread;
        QObject *app = nullptr;
        if (!isMedia) {
            app = new SetDefAppsThread(&m_dbusDefaultApps, mime, appName, mimeList);
            connect(t, &QThread::started, qobject_cast<SetDefAppsThread *>(app), &SetDefAppsThread::run);
        } else {
            app = new SetDefMediaThread(m_dbusDefaultMedia, mime, appName, mimeList);
            connect(t, &QThread::started, qobject_cast<SetDefMediaThread *>(app), &SetDefMediaThread::run);
        }
        connect(t, &QThread::finished, t, &QThread::deleteLater);
        connect(t, &QThread::finished, app, &QObject::deleteLater);
        app->moveToThread(t);
        t->start();

    });

    list->setFixedHeight(30 * list->count());
    defaultApps->setContent(list);

    qDebug() << "list:" << list << list->count();

    qDebug() << "m_arrowLineExpand" << defaultApps;


    return defaultApps;
}


void DefaultApps::setMediaOptionVisible(const bool visible)
{
    qDebug() << "reset visible to " << visible;

    m_modCDAudio->setVisible(visible);
    m_modDVDVideo->setVisible(visible);
    m_modMusicPlayer->setVisible(visible);
    m_modCamera->setVisible(visible);
    m_modSoftware->setVisible(visible);

    m_autoPlaySwitch->setChecked(visible);
    m_dbusDefaultMedia->EnableAutoOpen(visible);
}

void DefaultApps::resetDefaults()
{
    qDebug() << "reset";

    m_dbusDefaultApps.Reset().waitForFinished();
    m_dbusDefaultMedia->Reset().waitForFinished();
    setMediaOptionVisible(m_dbusDefaultMedia->autoOpen());
}

void DefaultApps::updateListCheckedIndex()
{
//    const QList<DefaultAppsCategory> &categoryList = m_appsList.keys();
//    for (const DefaultAppsCategory &category : categoryList)
//        updateCheckedItem(category);

    updateCheckedItem(Browser);
    updateCheckedItem(Mail);
    updateCheckedItem(Text);
    updateCheckedItem(Music);
    updateCheckedItem(Video);
    updateCheckedItem(Picture);
    updateCheckedItem(Terminal);
    updateCheckedItem(CD_Audio);
    updateCheckedItem(DVD_Video);
    updateCheckedItem(MusicPlayer);
    updateCheckedItem(Camera);
    updateCheckedItem(Software);
}

void DefaultApps::updateCheckedItem(const DefaultApps::DefaultAppsCategory &category)
{
    const QString &mime = getTypeByCategory(category);
    const QString &defApp = isMediaApps(category) ? m_dbusDefaultMedia->GetDefaultApp(mime)
                            : m_dbusDefaultApps.GetDefaultApp(mime);
    const QJsonObject &defaultApp = QJsonDocument::fromJson(defApp.toStdString().c_str()).object();
    const QString defAppValue = defaultApp.value("Id").toString();

    m_appsBtnList[category]->blockSignals(true);
    m_appsBtnList[category]->setCurrentSelected(defAppValue);
    m_appsBtnList[category]->blockSignals(false);
}


void DefaultApps::arrowLineExpandSetContent(QJsonArray json, int acategory, DArrowLineExpand *arrowLineApps)
{
    QString appDisplayName;
    QString appName;
    QString id;
    QString icon;

    DOptionList *m_list                         = new DOptionList;
    DefaultApps::DefaultAppsCategory category   = (DefaultApps::DefaultAppsCategory)acategory;
    const QString mime                          = getTypeByCategory(category);
    const bool isMedia                          = isMediaApps(category);
    QJsonArray appList                          = json;

    m_mediaGrp->addExpand(arrowLineApps);
    arrowLineApps->setContent(m_list);
    m_list->setFixedWidth(DCC::ModuleContentWidth);

    for (int i = 0; i != appList.size(); ++i) {
        QJsonObject jsonObj = appList.at(i).toObject();
        appName = jsonObj.take("Name").toString();

        appDisplayName = jsonObj.take("DisplayName").toString();
        id = jsonObj.take("Id").toString();
        icon = jsonObj.take("Icon").toString();

        DOption *option = new DOption;
        option->setName(appDisplayName.isEmpty() ? appName : appDisplayName);
        option->setValue(id);
        option->setFixedHeight(30);

//      if (!isMedia)
        {
            QPixmap pixmap(Helper::searchAppIcon(QStringList() << icon << "application-x-desktop", 16));
            option->setIcon(pixmap.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        m_list->addOption(option);
    }

    m_appsBtnList[category] = m_list;

    connect(m_list, &DOptionList::currentRowChanged, [ = ](int index) {
        const QStringList mimeList = getTypeListByCategory(category);
        const QString appName = appList.at(index).toObject().take("Id").toString();
        QThread *t = new QThread;
        QObject *app = nullptr;
        if (!isMedia) {
            app = new SetDefAppsThread(&m_dbusDefaultApps, mime, appName, mimeList);

            connect(t, &QThread::started, qobject_cast<SetDefAppsThread *>(app), &SetDefAppsThread::run);

        } else {
            app = new SetDefMediaThread(m_dbusDefaultMedia, mime, appName, mimeList);
            connect(t, &QThread::started, qobject_cast<SetDefMediaThread *>(app), &SetDefMediaThread::run);
        }
        app->moveToThread(t);
        t->start();
        connect(t, &QThread::finished, app, &QObject::deleteLater);
        connect(t, &QThread::finished, t, &QThread::deleteLater);
    });

    m_list->setFixedHeight(30 * m_list->count());

    updateCheckedItem(category);
}


void DefaultApps::createTask()
{
    qDebug() << "createTask start";
    QThread *workThread = new QThread;
    RunnableTask *task = new RunnableTask(m_taskMap, &m_appsBtnList);
    connect(workThread, &QThread::started, task, &RunnableTask::run);
    connect(task, &RunnableTask::appListReady, this, &DefaultApps::arrowLineExpandSetContent);
    connect(workThread, &QThread::finished, task, &QObject::deleteLater);
    connect(workThread, &QThread::finished, workThread, &QThread::deleteLater);
    task->moveToThread(workThread);
    workThread->start();
    qDebug() << "createTask end";
}

void DefaultApps::lazyLoad()
{
    qDebug() << "m_dbusDefaultMedia start";
    const bool isMediaOpen = m_dbusDefaultMedia->autoOpen();
    qDebug() << "setMediaOptionVisible start";
    setMediaOptionVisible(isMediaOpen);
    qDebug() << "insert start";

    m_taskMap.insert(Browser, m_modBrowser);
    m_taskMap.insert(Mail, m_modMail);
    m_taskMap.insert(Text, m_modText);
    m_taskMap.insert(Music, m_modMusic);
    m_taskMap.insert(Video, m_modVideo);
    m_taskMap.insert(Picture, m_modPicture);
    m_taskMap.insert(Terminal, m_modTerminal);
    m_taskMap.insert(CD_Audio, m_modCDAudio);
    m_taskMap.insert(DVD_Video, m_modDVDVideo);
    m_taskMap.insert(MusicPlayer, m_modMusicPlayer);
    m_taskMap.insert(Camera, m_modCamera);
    m_taskMap.insert(Software, m_modSoftware);
    createTask();
    qDebug() << "lazyLoad end";
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
    for (const QString &mime : list) {
        dbus->SetDefaultApp(mime, appName).waitForFinished();
    }
//    qDebug() << "set def app : " << appName << " to " << list;
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
    for (const QString &mime : list) {
        dbus->SetDefaultApp(mime, appName).waitForFinished();
    }
//    qDebug() << "set def media : " << appName << " to " << list;
}

bool DefaultApps::isMediaApps(const DefaultApps::DefaultAppsCategory &category) const
{
    switch (category) {
    case Browser:
    case Mail:
    case Text:
    case Music:
    case Video:
    case Picture:
    case Terminal:      return false;

    case CD_Audio:
    case DVD_Video:
    case MusicPlayer:
    case Camera:
    case Software:      return true;
    default:;
    }

    // for remove complier warnings.
    return true;
}

const QString DefaultApps::getTypeByCategory(const DefaultApps::DefaultAppsCategory &category)
{
    switch (category) {
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

