/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DISKCONTENT_H
#define DISKCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QFile>
#include <QDebug>
#include "dbus/dbusdiskmount.h"
#include <dde-dock/dockconstants.h>
#include <dde-dock/dockpluginproxyinterface.h>
#include "diskitem.h"

#include <dboxwidget.h>
#include <dlistwidget.h>

DWIDGET_USE_NAMESPACE

class DiskContent : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit DiskContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent = 0);
    ~DiskContent();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void initStyleSheet();
    void initDiskMount();

    void updateMountDisks();

private:
    QMap<QString, DiskItem *> m_itemList;
    QString m_id = "";
    DockPluginProxyInterface *m_proxy = NULL;
    DBusDiskMount *m_diskMount = NULL;
    DListWidget *m_listWidget = NULL;
};

#endif // DISKCONTENT_H
