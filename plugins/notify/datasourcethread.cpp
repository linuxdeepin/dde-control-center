/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "datasourcethread.h"

DataSourceThread::DataSourceThread(QObject *parent)
{
    Q_UNUSED(parent);
}

DataSourceThread::~DataSourceThread() {
    m_process->deleteLater();
}

void DataSourceThread::run() {
    m_process = new QProcess();
    m_process->start("dbus-monitor",QStringList("interface='org.freedesktop.Notifications',member='Notify',type='method_call',eavesdrop='true'"));
    connect(m_process, &QProcess::readyReadStandardOutput, [=] {
        QTimer::singleShot(100, this, [=] {
            QString string = m_process->readAllStandardOutput();
            QRegularExpression regularExpression("string.\\\"(?<string>.*)\\\"\\n");
            int index = 0;
            QStringList list;
            QRegularExpressionMatch match;
            do {
                match = regularExpression.match(string, index);
                if(match.hasMatch()) {
                    index = match.capturedEnd();
                    qDebug()<<"("<<match.capturedStart()<<","<<index<<") "<<match.captured("string");
                    list.append(match.captured("string"));
                } else {
                    break;
                }
            } while(index < string.length());
            if (list.size() > 3){
                if (list[0].isEmpty()) {
                    list.clear();
                } else {
                    QJsonObject json;
                    json.insert("appName", list[0]);
                    json.insert("appIcon", list[1]);
                    json.insert("summary", list[2]);
                    json.insert("body"   , list[3]);
                    QJsonDocument document;
                    document.setObject(json);
                    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
                    emit DataSourceThread::ValueChanged(byte_array);
                }
            }
        });
    });
    m_process->waitForFinished(-1);
}
