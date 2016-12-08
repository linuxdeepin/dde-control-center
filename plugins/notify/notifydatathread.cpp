/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifydatathread.h"

NotifyDataThread::NotifyDataThread(QThread *parent):
    QThread(parent)
{

}

NotifyDataThread::~NotifyDataThread()
{

}

void NotifyDataThread::run()
{
    m_process.start("dbus-monitor", QStringList("interface='org.freedesktop.Notifications',member='Notify',type='method_call',eavesdrop='true'"));
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &NotifyDataThread::processReadyReadStandardOutput);
    m_process.waitForFinished(-1);
}

void NotifyDataThread::processReadyReadStandardOutput()
{
    QTimer::singleShot(100, this, [ = ] {
        QString string = m_process.readAllStandardOutput();
        QRegularExpression regularExpression("string.\\\"(?<string>.*)\\\"\\n");
        int index = 0;
        QStringList list;
        QRegularExpressionMatch match;
        do {
            match = regularExpression.match(string, index);
            if (match.hasMatch())
            {
                index = match.capturedEnd();
                qDebug() << "(" << match.capturedStart() << "," << index << ") " << match.captured("string");
                list.append(match.captured("string"));
            } else {
                break;
            }
        } while (index < string.length());
        converToJson(list);
    });
}

void NotifyDataThread::converToJson(QStringList &list)
{
    if (list.size() > 3) {
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
            emit NotifyDataThread::ValueChanged(byte_array);
        }
    }
}
