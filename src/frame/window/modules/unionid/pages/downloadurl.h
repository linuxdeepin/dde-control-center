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
namespace unionid {
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
