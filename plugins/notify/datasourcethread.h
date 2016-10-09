/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DATASOURCETHREAD_H
#define DATASOURCETHREAD_H

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
class DataSourceThread : public QThread
{
    Q_OBJECT
public:
    explicit DataSourceThread(QObject *parent = 0);
    ~DataSourceThread();
    void run();

signals:
    void ValueChanged(QByteArray s);

public slots:

private:
        QProcess *m_process;
};

#endif // DATASOURCETHREAD_H
