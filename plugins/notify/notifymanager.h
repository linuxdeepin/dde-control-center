/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>
#include <QWidget>
#include "notifyviewer.h"
#include <QVBoxLayout>
#include <QTime>
#include <org_freedesktop_notifications.h>
#include <dimagebutton.h>
#include <QList>
#include <QJsonObject>

using org::freedesktop::Notifications;

class NotifyManager : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyManager(QWidget *parent = 0);
    ~NotifyManager();

public slots:
    void checkNotify();
    void onNotifyAdded(const QString &value);

private slots:
    void onNotifyGetAllFinished(QDBusPendingCallWatcher *w);
    void onNotifyAdd(const QJsonObject &value);
    void onNotifyRemove(const QString &id);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void onCloseAllItem();

private:
    Viewer *m_viewer;
    QVBoxLayout *m_mainLayout;
    DImageButton *m_clearButton;
    QMap<Viewer*, QJsonObject> m_viewerList;
    Notifications *m_dbus;
};

#endif // NOTIFYMANAGER_H
