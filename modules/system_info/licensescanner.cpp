/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QLocale>

#include "licensescanner.h"

QString getLocalizedValue(QSettings & settings, QString key)
{
    QString realKey = QString("License/%1").arg(key);
    QString localizedRealKey = QString("License/%1[%2]").arg(key).arg(QLocale::system().name());

    return settings.value(localizedRealKey, settings.value(realKey)).toString();
}

QString getLocalizedlicenseBody(QDir licenseDir)
{
    QString licenseFile = licenseDir.filePath("license.txt");
    QString localizedlicenseFile = licenseDir.filePath(QString("license_%1.txt").arg(QLocale::system().name()));

    QFile file(licenseFile);
    if (QFile::exists(localizedlicenseFile)) {
        file.setFileName(localizedlicenseFile);
    }

    if (file.open(QIODevice::ReadOnly)) {
        QString body = file.readAll();
        file.close();

        return body;
    } else {
        return "";
    }
}

QMap<QString, QString> licenseScanner::scan()
{
    QMap<QString, QString> result;

    QDir licensesDir("/usr/share/deepin/licenses");

    const QStringList dirs = licensesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (QString dirName : dirs) {
        if (licensesDir.cd(dirName)) {
            QString confFile = licensesDir.filePath("license.conf");
            if (QFile::exists(confFile)) {
                QSettings config(confFile, QSettings::IniFormat);
                config.setIniCodec("utf-8");

                QString title = getLocalizedValue(config, "Title");
                QString body = getLocalizedlicenseBody(licensesDir);

                result[title] = body;
            }

            licensesDir.cdUp();
        }
    }

    return result;
}
