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
#include <QEventLoop>
#include <QFile>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

DownloadUrl::DownloadUrl(QObject *parent)
    : QObject(parent)
    , m_manager(nullptr)
    , m_isReady(true)
{

}

DownloadUrl::~DownloadUrl()
{
    if (m_manager != nullptr)
        m_manager->deleteLater();
}

void DownloadUrl::downloadFileFromURL(const QString &url, const QString &filePath)
{
    const QString fileName = filePath + url.right(url.size() - url.lastIndexOf("/"));
    qDebug() << "download " << url << " to " << fileName;
    if (QFile::exists(fileName)) {
        Q_EMIT fileDownloaded(fileName);
        return;
    }

    if (!m_isReady)
        return;
    m_isReady = false;

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

    connect(m_manager, &QNetworkAccessManager::finished, this, &DownloadUrl::onDownloadFileComplete);

    m_manager->get(request);
}

void DownloadUrl::onDownloadFileComplete(QNetworkReply *reply)
{
    if (m_file == nullptr)
        return;

    if (m_file->isWritable())
        m_file->write(reply->readAll());

    m_file->close();
    qDebug() << " m_file fileName = " << m_file->fileName();
    Q_EMIT fileDownloaded(m_file->fileName());

    delete m_file;
    m_file = nullptr;
    m_isReady = true;
}

bool DownloadUrl::downloadUrl(const QString &url, const QString &fileName)
{
    QNetworkAccessManager manager;
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        return false;
    }

    QFile f(fileName);
    qDebug() << fileName;
    if(!f.open(QIODevice::WriteOnly))
        return false;
    f.write(reply->readAll());
    f.close();
    delete reply;
    return true;
}
