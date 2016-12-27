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
#include <QDBusMessage>

class NotifyData : public QObject
{
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_OBJECT
public:
    explicit NotifyData(QObject *parent = 0);
    ~NotifyData();

public Q_SLOTS:
    void Notify(const QString &app_name, uint replaces_id,
                const QString &app_icon, const QString &summary,
                const QString &body, const QStringList &actions,
                const QVariantHash &hints, int expire_timeout);

signals:
    void ValueChanged(const QByteArray &s) const;
};

#endif // NOTIFYDATA_H
