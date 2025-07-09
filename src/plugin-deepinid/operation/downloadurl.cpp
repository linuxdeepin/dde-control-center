// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "downloadurl.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QPixmap>
#include <QDebug>
#include <QEventLoop>

DownloadUrl::DownloadUrl(QObject *parent)
    : QObject(parent)
    , m_isReady(true)
{

}

DownloadUrl::~DownloadUrl()
{

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
    qDebug() << "Download file from URL, URL: " << url << ", fileName: " << fileName << ", ready: " << m_isReady;

    if (!m_isReady)
        return;
    m_isReady = false;
    fileName = fileName + ".tmp";
    m_retryMap.insert(fileName, url);

    QNetworkAccessManager manager;
    QNetworkRequest request;
    QEventLoop loop;
    QTimer timer;

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(config);

    QNetworkReply *pReply = manager.get(request);
    connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    if (pReply) {
        timer.setSingleShot(true);
        timer.start(10 * 1000);
        loop.exec();
        if (QNetworkReply::NoError == pReply->error()) {
            QByteArray replyData = pReply->readAll();

            QFile file(fileName);
            file.open(QIODevice::WriteOnly);
            if (!file.isOpen()) {
                m_isReady = true;
                return;
            }

            if (file.write(replyData) <= 0) {
                qWarning() << "On download file failed, reply data is empty: " << url;
                file.remove();
            } else {
                file.close();
                if (m_retryMap.contains(file.fileName()))
                    m_retryMap.remove(file.fileName());

                const QString fileName = file.fileName().left(file.fileName().length() - 4);
                QFile::rename(file.fileName(), fileName);
                qInfo() << "On download file, file name: " << fileName;
                Q_EMIT fileDownloaded(fileName);
            }
        } else {
            qWarning() << "Download failed:" << url << pReply->errorString();
            QString url = pReply->url().toString();
            if (m_retryMap.value(fileName) != url)
                qWarning() << "Download file error, url: " << m_retryMap.value(fileName) << " is different from " << url;
        }

        pReply->close();
        pReply->deleteLater();
        pReply = nullptr;
        m_isReady = true;
        if (!m_retryMap.isEmpty())
            onDownloadFileError(m_retryMap.begin().key(), m_retryMap.begin().value());
    }
}

void DownloadUrl::onDownloadFileError(const QString &url, const QString &fileName)
{
    qDebug() << "Download file error, will try again after 20 seconds if url is valid";
    if (url.isEmpty())
        return;

    QTimer::singleShot(20000, this, [=] {
        qInfo() << "Retry to download file " << url << " to " << fileName;
        downloadFileFromURL(url, fileName, true);
    });
}
