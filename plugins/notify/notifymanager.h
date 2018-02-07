/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>
#include <QWidget>
#include "notifyviewer.h"
#include <QVBoxLayout>
#include <org_freedesktop_notifications.h>
#include <dimagebutton.h>
#include <QList>

using org::freedesktop::Notifications;

static const QStringList Directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
static const QString CacheFolder = Directory.first() + "/.cache/deepin/deepin-notifications/";

class NotifyManager : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyManager(QWidget *parent = 0);
    ~NotifyManager();

public slots:
    void onNotifyAdded(const QString &value);

private slots:
    void onNotifyGetAllFinished(QDBusPendingCallWatcher *w);
    Viewer* onNotifyAdd(const QJsonObject &value);
    void onNotifyRemove(const QString &id);
    void onLoadAgain();

protected:
    void paintEvent(QPaintEvent *e);

private:
    void onCloseAllItem();

private:
    DImageButton *m_clearButton;
    QMap<Viewer*, QJsonObject> m_viewerList;
    Notifications *m_dbus;
    QVBoxLayout *m_connectLayout;
    QJsonArray m_dataJsonArray;
    int m_checkIndex;
};

#endif // NOTIFYMANAGER_H
