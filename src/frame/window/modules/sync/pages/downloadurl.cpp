/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "downloadurl.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QPixmap>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

DownloadUrl::DownloadUrl(QObject *parent)
    : QObject(parent)
    , m_manager(nullptr)
    , m_file(nullptr)
    , m_isReady(true)
{

}

DownloadUrl::~DownloadUrl()
{
    if (m_manager != nullptr)
        m_manager->deleteLater();
}

void DownloadUrl::downloadFileFromURL(const QString &url, const QString &filePath, bool fullname)
{
    if (url.isEmpty())
        return;

    QString fileName;
    fileName = fullname ? filePath : filePath + url.right(url.size() - url.lastIndexOf("/"));
    if (fileName.contains("default.png", Qt::CaseInsensitive)) {
        fileName = fileName.remove("png").append("svg");
    }
    qDebug() << " download " << url << " to " << fileName << " ready = " << m_isReady;

    if (!m_isReady)
        return;
    m_isReady = false;

    m_retryMap.insert(fileName, url);

    m_file = new QFile();
    m_file->setFileName(fileName);
    m_file->open(QIODevice::WriteOnly);
    if (!m_file->isOpen()) {
        m_isReady = true;
        return;
    }

    if (m_manager == nullptr)
        m_manager = new QNetworkAccessManager;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    connect(m_manager, &QNetworkAccessManager::finished, this, &DownloadUrl::onDownloadFileComplete);

    m_manager->get(request);

    if (QFile::exists(fileName)) {
        QPixmap pxmap;
        if (pxmap.load(fileName)) {
            Q_EMIT fileDownloaded(fileName);
        }
    }
}

void DownloadUrl::onDownloadFileComplete(QNetworkReply *reply)
{
    //return receiving redundant finished signal
    if (m_file == nullptr)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "network error = " << reply->error();
        QString url = reply->url().toString();

        if (m_retryMap.value(m_file->fileName()) != url)
            qDebug() << " mfile url " << m_retryMap.value(m_file->fileName()) << " != " << url;

        m_file->remove();
        delete m_file;
        m_file = nullptr;
        reply->deleteLater();
        m_isReady = true;
        onDownloadFileError(url, m_retryMap.key(url));
        return;
    }

    if (m_file->write(reply->readAll()) <= 0) {
        qDebug() << " write error " << m_file->fileName();
        m_file->remove();
    } else {
        m_file->close();
        if (m_retryMap.contains(m_file->fileName()))
            m_retryMap.remove(m_file->fileName());
        qDebug() << " m_file fileName = " << m_file->fileName();
        Q_EMIT fileDownloaded(m_file->fileName());
    }

    delete m_file;
    m_file = nullptr;
    reply->deleteLater();
    m_isReady = true;
    if (!m_retryMap.isEmpty())
        onDownloadFileError(m_retryMap.begin().key(), m_retryMap.begin().value());
}

void DownloadUrl::onDownloadFileError(const QString &url, const QString &fileName)
{
    if (url.isEmpty())
        return;

    QTimer::singleShot(20000, this, [=] {
        qDebug() << " retry to download file " + url << " to " << fileName;
        downloadFileFromURL(url, fileName, true);
    });
}
