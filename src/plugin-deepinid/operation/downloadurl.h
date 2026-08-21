// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QMap>

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
    void onDownloadFileError(const QString &url, const QString &fileName);

private:
    bool m_isReady;
    QMap<QString, QString> m_retryMap;
};
