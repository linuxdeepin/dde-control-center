/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "runnabletask.h"
#include "constants.h"


RunnableTask::RunnableTask(QMap<DefaultApps::DefaultAppsCategory, DArrowLineExpand*> taskMap,
                           QMap<DefaultApps::DefaultAppsCategory, DOptionList*> *appBtnMap):
    m_appsBtnMap(appBtnMap),m_taskMap(taskMap)
{

}

void RunnableTask::run()
{

    QMapIterator<DefaultApps::DefaultAppsCategory, DArrowLineExpand*> i(m_taskMap);
    while(i.hasNext())
    {
        i.next();
        createDefaultAppsExpand(i.key(), i.value());
    }

}


void RunnableTask::updateCheckedItem(const DefaultApps::DefaultAppsCategory &category)
{
     const QString &mime = getTypeByCategory(category);
     const QString &defApp = isMediaApps(category) ? m_dbusDefaultMedia.GetDefaultApp(mime)
                                                   : m_dbusDefaultApps.GetDefaultApp(mime);
     const QJsonObject &defaultApp = QJsonDocument::fromJson(defApp.toStdString().c_str()).object();
     const QString defAppValue = defaultApp.value("Id").toString();

     (*m_appsBtnMap)[category]->blockSignals(true);
     (*m_appsBtnMap)[category]->setCurrentSelected(defAppValue);
     (*m_appsBtnMap)[category]->blockSignals(false);
}

DArrowLineExpand *RunnableTask::createDefaultAppsExpand(const DefaultApps::DefaultAppsCategory &category, DArrowLineExpand *arrowLineApps)
{

    const QString mime = getTypeByCategory(category);
    QString appListJson;

    if (isMediaApps(category))
        appListJson = m_dbusDefaultMedia.ListApps(mime);
    else
        appListJson = m_dbusDefaultApps.ListApps(mime);


    QJsonArray appList = QJsonDocument::fromJson(appListJson.toStdString().c_str()).array();
    emit appListReady(appList , (int)category , arrowLineApps);

    return arrowLineApps;
}


bool RunnableTask::isMediaApps(const DefaultApps::DefaultAppsCategory &category) const
{
    switch (category)
    {
    case DefaultApps::DefaultAppsCategory::Browser:
    case DefaultApps::DefaultAppsCategory::Mail:
    case DefaultApps::DefaultAppsCategory::Text:
    case DefaultApps::DefaultAppsCategory::Music:
    case DefaultApps::DefaultAppsCategory::Video:
    case DefaultApps::DefaultAppsCategory::Picture:
    case DefaultApps::DefaultAppsCategory::Terminal:      return false;
    case DefaultApps::DefaultAppsCategory::CD_Audio:
    case DefaultApps::DefaultAppsCategory::DVD_Video:
    case DefaultApps::DefaultAppsCategory::MusicPlayer:
    case DefaultApps::DefaultAppsCategory::Camera:
    case DefaultApps::DefaultAppsCategory::Software:      return true;
    default:;
    }

    // for remove complier warnings.
    return true;
}

const QString RunnableTask::getTypeByCategory(const DefaultApps::DefaultAppsCategory &category)
{
    switch (category)
    {
    case DefaultApps::DefaultAppsCategory::Browser:       return "x-scheme-handler/http";
    case DefaultApps::DefaultAppsCategory::Mail:          return "x-scheme-handler/mailto";
    case DefaultApps::DefaultAppsCategory::Text:          return "text/plain";
    case DefaultApps::DefaultAppsCategory::Music:         return "audio/mpeg";
    case DefaultApps::DefaultAppsCategory::Video:         return "video/mp4";
    case DefaultApps::DefaultAppsCategory::Picture:       return "image/jpeg";
    case DefaultApps::DefaultAppsCategory::Terminal:      return "application/x-terminal";
    case DefaultApps::DefaultAppsCategory::CD_Audio:      return "x-content/audio-cdda";
    case DefaultApps::DefaultAppsCategory::DVD_Video:     return "x-content/video-dvd";
    case DefaultApps::DefaultAppsCategory::MusicPlayer:   return "x-content/audio-player";
    case DefaultApps::DefaultAppsCategory::Camera:        return "x-content/image-dcf";
    case DefaultApps::DefaultAppsCategory::Software:      return "x-content/unix-software";
    }

    return QString();
}

const QStringList RunnableTask::getTypeListByCategory(const DefaultApps::DefaultAppsCategory &category)
{

    switch (category)
    {
    case DefaultApps::DefaultAppsCategory::Browser:       return QStringList() << "x-scheme-handler/http" << "x-scheme-handler/ftp"
                                                                               << "x-scheme-handler/https" << "text/html" << "text/xml"
                                                                               << "text/xhtml_xml" << "text/xhtml+xml";
    case DefaultApps::DefaultAppsCategory::Mail:          return QStringList() << "x-scheme-handler/mailto" << "message/rfc822"
                                                                               << "application/x-extension-eml" << "application/x-xpinstall";
    case DefaultApps::DefaultAppsCategory::Text:          return QStringList() << "text/plain";
    case DefaultApps::DefaultAppsCategory::Music:         return QStringList() << "audio/mpeg" << "audio/mp3" << "audio/x-mp3"
                                                                               << "audio/mpeg3" << "audio/x-mpeg-3"<< "audio/x-mpeg"
                                                                               << "audio/flac" << "audio/x-flac" << "application/x-flac"
                                                                               << "audio/ape" << "audio/x-ape" << "application/x-ape" << "audio/ogg"
                                                                               << "audio/x-ogg" << "audio/musepack" << "application/musepack"
                                                                               << "audio/x-musepack" << "application/x-musepack" << "audio/mpc"
                                                                               << "audio/x-mpc" << "audio/vorbis" << "audio/x-vorbis" << "audio/x-wav"
                                                                               << "audio/x-ms-wma";
    case DefaultApps::DefaultAppsCategory::Video:         return QStringList() << "video/mp4" << "audio/mp4" << "audio/x-matroska" << "video/x-matroska"
                                                                               << "application/x-matroska" << "video/avi" << "video/msvideo" << "video/x-msvideo"
                                                                               << "video/ogg" << "application/ogg" << "application/x-ogg" << "video/3gpp"
                                                                               << "video/3gpp2" << "video/flv" << "video/x-flv" << "video/x-flic" << "video/mpeg"
                                                                               << "video/x-mpeg" << "video/x-ogm" << "application/x-shockwave-flash"
                                                                               << "video/x-theora" << "video/quicktime" << "video/x-ms-asf"
                                                                               << "application/vnd.rn-realmedia" << "video/x-ms-wmv";
    case DefaultApps::DefaultAppsCategory::Picture:       return QStringList() << "image/jpeg" << "image/pjpeg" << "image/bmp"
                                                                               << "image/x-bmp" << "image/png" << "image/x-png"
                                                                               << "image/tiff" << "image/svg+xml" << "image/x-xbitmap"
                                                                               << "image/gif" << "image/x-xpixmap";
    case DefaultApps::DefaultAppsCategory::Terminal:      return QStringList() << "application/x-terminal";
    case DefaultApps::DefaultAppsCategory::CD_Audio:      return QStringList() << "x-content/audio-cdda";
    case DefaultApps::DefaultAppsCategory::DVD_Video:     return QStringList() << "x-content/video-dvd";
    case DefaultApps::DefaultAppsCategory::MusicPlayer:   return QStringList() << "x-content/audio-player";
    case DefaultApps::DefaultAppsCategory::Camera:        return QStringList() << "x-content/image-dcf";
    case DefaultApps::DefaultAppsCategory::Software:      return QStringList() << "x-content/unix-software";
    }

    return QStringList();
}

