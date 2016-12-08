/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NOTIFYDATATHREAD_H
#define NOTIFYDATATHREAD_H

#include <QObject>
#include <QWidget>
#include <QThread>
#include <QProcess>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QTimer>

#include <QDebug>
class NotifyDataThread : public QThread
{
    Q_OBJECT
public:
    explicit NotifyDataThread(QThread *parent = 0);
    ~NotifyDataThread();
    void run();

signals:
    void ValueChanged(const QByteArray &s) const;

public slots:
    void processReadyReadStandardOutput();
    void converToJson(QStringList &list);

private:
    QProcess m_process;
};

#endif // NOTIFYDATATHREAD_H
