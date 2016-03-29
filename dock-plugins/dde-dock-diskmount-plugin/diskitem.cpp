/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "diskitem.h"

const int DISK_ITEM_HEIGHT = 80;
const int DISK_ITEM_WIDTH = 220;
DiskItem::DiskItem(const QString &id, DBusDiskMount *diskMount, QWidget *parent)
    : QLabel(parent), m_id(id)
{
    notifyInterface = new QDBusInterface("org.freedesktop.Notifications",
                                                         "/org/freedesktop/Notifications",
                                                         "org.freedesktop.Notifications",
                                                         QDBusConnection::sessionBus());
    connect(notifyInterface, SIGNAL(ActionInvoked(uint,QString)), SLOT(slotRetry(uint,QString)));

    m_diskMount = diskMount;
    connect(diskMount, &DBusDiskMount::DiskListChanged, this, &DiskItem::updateData);
    connect(diskMount, &DBusDiskMount::Error, this, &DiskItem::umountDiskFailed);
    this->setFixedSize(DISK_ITEM_WIDTH, DISK_ITEM_HEIGHT);
    initWidgets();

    updateData();
}

void DiskItem::updateData()
{
    DiskInfoList infoList = m_diskMount->diskList();
    foreach (DiskInfo info, infoList)
    {
        if (info.id == m_id)
        {
            m_diskUuid = info.id;

            m_diskType = info.type;
            m_titleLabel->setText(info.name);
            m_diskIcon->setIcon(info.icon);
            m_diskIcon->setMountPoint(info.mountPoint);
            m_progressLabel->setText(bitToHuman(info.used) + "/" + bitToHuman(info.total));
            m_usedBar->setValue(100 * (double(info.used) / info.total));
        }
    }
}

void DiskItem::sendNotification(const QString &title, const QString &msg)
{
    qDebug() << "[DiskMountPlugin] send message:" << title;

    notifyInterface->asyncCall("Notify",
                           "Disk Mount",
                           uint(0),
                           "diskmount",
                           title,
                           msg,
                           QStringList() << m_diskUuid << tr("Retry"),
                           QVariantMap(),
                           0);
}

void DiskItem::umountDisk()
{
    qDebug() << "[DiskMountPlugin] Try to umount Disk: "<< m_diskUuid;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_diskMount->Unmount(m_diskUuid));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher]{
        if(!watcher->reply().arguments().first().toBool()){
            umountDiskFailed(m_id, "Umount disk error.");
        }

        watcher->deleteLater();
    });
}

void DiskItem::slotRetry(uint, QString id)
{
    if(id == m_diskUuid){
        umountDisk();
    }
}

void DiskItem::updateUnMountButtonState(bool normal)
{
    QString stateStr = normal ? "" : "_fail";
    m_unMountButton->setNormalPic(QString("://Resource/images/unmount_button_normal%1.png").arg(stateStr));
    m_unMountButton->setHoverPic(QString("://Resource/images/unmount_button_hover%1.png").arg(stateStr));
    m_unMountButton->setPressPic(QString("://Resource/images/unmount_button_press%1.png").arg(stateStr));
}

void DiskItem::umountDiskFailed(const QString &uuid, const QString &reason)
{
    Q_UNUSED(reason);

    if (m_id != uuid)
        return;

    updateUnMountButtonState(false);

    QString disk_type_name;

    if(m_diskType == "removable"){
        disk_type_name = tr("removable disk");
    }else if(m_diskType == "network"){
        disk_type_name = tr("network disk");
    }else{
        disk_type_name = tr("local disk");
    }

    sendNotification("", QString(tr("Failed to uninstall %1 \" %2 \", please retry."))
                     .arg(disk_type_name)
                     .arg(m_titleLabel->text()));
}

void DiskItem::initWidgets()
{
    const int itemSpacing = 5;

    m_diskIcon = new DiskIcon(this);
    m_diskIcon->move(0, 0);

    m_titleLabel = new QLabel("Disk",this);
    m_titleLabel->setObjectName("DiskTitle");
    m_titleLabel->setFixedSize(width(), 20);
    m_titleLabel->move(m_diskIcon->x() + m_diskIcon->width() + itemSpacing, m_diskIcon->y());

    m_progressLabel = new QLabel("0GB/0GB", this);
    m_progressLabel->setObjectName("DiskProgressLabel");
    m_progressLabel->setFixedSize(width() - m_diskIcon->width() - 10, 20);
    m_progressLabel->move(m_titleLabel->x(), m_titleLabel->y() + m_titleLabel->height());

    m_usedBar = new QProgressBar(this);
    m_usedBar->setRange(0,100);
    m_usedBar->setTextVisible(false);
    m_usedBar->setFixedSize(m_progressLabel->width(), 2);
    m_usedBar->move(m_progressLabel->x(),m_progressLabel->y() + m_progressLabel->height() + itemSpacing);

    m_unMountButton = new DImageButton(this);
    m_unMountButton->setFixedSize(20, 20);
    updateUnMountButtonState(true);
    m_unMountButton->move(width() - m_unMountButton->width() - itemSpacing, m_usedBar->y() - m_unMountButton->height() - itemSpacing);
    connect(m_unMountButton, &DImageButton::clicked, this, &DiskItem::umountDisk);
}

QString DiskItem::bitToHuman(qint64 value)
{
    const qint64 mSize = 1000;
    const qint64 gSize = mSize * 1000;

    if (value >= gSize)
        return QString::number(double(value) / gSize, 'f', 2) + "GB";
    else if (value >= mSize)
        return QString::number(double(value) / mSize, 'f', 2) + "MB";
    else
        return QString::number(value) + "KB";
}
