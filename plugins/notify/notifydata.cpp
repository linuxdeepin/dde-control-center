/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifydata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QApplication>

NotifyData::NotifyData(QObject *parent)
    : QObject(parent)
{
    QProcess *process = new QProcess(this);
    process->setProgram("python");
#ifdef QT_DEBUG
    QDir appDir( QApplication::applicationDirPath() );
    appDir.cd("plugins/notify");
    process->setArguments(QStringList() << appDir.filePath("notify-monitor"));
#else
    process->setArguments(QStringList() << "/usr/lib/dde-control-center/plugins/bin/notify-monitor");
#endif

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process] {
        emit dataReceived(process->readAllStandardOutput());
    });
    connect(process, &QProcess::errorOccurred, this, [this, process] {
        qWarning() << process->errorString();
    });

    process->start();
}

NotifyData::~NotifyData()
{

}
