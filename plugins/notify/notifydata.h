/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NOTIFYDATA_H
#define NOTIFYDATA_H

#include <QObject>

class NotifyData : public QObject
{
    Q_OBJECT
public:
    explicit NotifyData(QObject *parent = 0);
    ~NotifyData();

signals:
    void dataReceived(const QByteArray &s) const;
};

#endif // NOTIFYDATA_H
