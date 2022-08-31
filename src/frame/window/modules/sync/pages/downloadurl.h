// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QObject>
#include <QMap>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QFile;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace sync {
class DownloadUrl : public QObject
{
    Q_OBJECT
public:
    explicit DownloadUrl(QObject *parent = nullptr);
    virtual ~DownloadUrl();
    void downloadFileFromURL(const QString &url, const QString &filePath, bool fullname = false);

Q_SIGNALS:
    void fileDownloaded(const QString &fileName);

public Q_SLOTS:
    void onDownloadFileComplete(QNetworkReply *reply);
    void onDownloadFileError(const QString &url, const QString &fileName);

private:
    QNetworkAccessManager *m_manager;
    QFile *m_file;
    bool m_isReady;
    QMap<QString, QString> m_retryMap;
};
}
}
