/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DISKITEM_H
#define DISKITEM_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QDebug>
#include "dimagebutton.h"
#include "dbus/dbusdiskmount.h"
#include "diskicon.h"

DWIDGET_USE_NAMESPACE

class DiskItem : public QLabel
{
    Q_OBJECT
public:
    explicit DiskItem(const QString &id, DBusDiskMount *diskMount , QWidget *parent = 0);

public slots:
    void updateData();

private:
    void initWidgets();
    QString bitToHuman(qint64 value);
    void sendNotification(const QString &title, const QString &msg);

private slots:
    void umountDisk();
    void slotRetry(uint, QString);
    void updateUnMountButtonState(bool normal);
    void umountDiskFailed(const QString &id, const QString &reason);

private:
    DBusDiskMount *m_diskMount = NULL;
    QString m_id = "";
    QString m_diskUuid = "";
    QString m_diskType;
    QLabel *m_titleLabel = NULL;
    DiskIcon *m_diskIcon = NULL;
    QLabel *m_progressLabel = NULL;
    QProgressBar *m_usedBar = NULL;
    QDBusInterface *notifyInterface;
    DImageButton *m_unMountButton;
};

#endif // DISKITEM_H
