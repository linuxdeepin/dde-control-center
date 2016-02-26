/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QResizeEvent>
#include "diskcontent.h"

const int DISK_ITEM_HEIGHT = 80;
const int DISK_ITEM_WIDTH = 220;
const int DISK_ITEM_MARGIN = 10;
const int DISK_ITEM_SPACING = 6;
const int MAX_VISABLE_COUNT = 4;
DiskContent::DiskContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent)
    : DVBoxWidget(parent), m_id(id), m_proxy(proxy)
{
    initStyleSheet();
    initDiskMount();

    m_listWidget = new DListWidget;
    m_listWidget->setItemSize(DISK_ITEM_WIDTH, DISK_ITEM_HEIGHT);
    m_listWidget->setEnableVerticalScroll(true);
    m_listWidget->setMaximumHeight(360);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setSpacing(DISK_ITEM_SPACING);
    m_listWidget->setContentsMargins(DISK_ITEM_MARGIN, DISK_ITEM_MARGIN + DISK_ITEM_SPACING, DISK_ITEM_MARGIN, DISK_ITEM_MARGIN);
    addWidget(m_listWidget);

    updateMountDisks();
}

void DiskContent::initStyleSheet()
{
    QFile file("://Resource/qss/disk.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);

        file.close();
    }
    else
        qWarning() << "[DiskMountPlugin] Open  style file errr!";
}

void DiskContent::initDiskMount()
{
    m_diskMount = new DBusDiskMount(this);
    connect(m_diskMount,&DBusDiskMount::DiskListChanged,this,&DiskContent::updateMountDisks);
}

void DiskContent::updateMountDisks()
{
    DiskInfoList tmpList = m_diskMount->diskList();
    QStringList idList;
    for (DiskInfo info : tmpList)
    {
        if (info.canUnmount)
            idList << info.uUID;
    }

    for (DiskInfo info : tmpList)
    {
        if (info.canUnmount && m_itemList.keys().indexOf(info.uUID) == -1)//New Item
        {
            DiskItem * item = new DiskItem(info.uUID,m_diskMount, this);
            m_itemList.insert(info.uUID,item);
            m_listWidget->addWidget(item);

            qDebug() << "[DiskMountPlugin] Disk Mounted:" << info.uUID;
        }
    }

    for (QString id : m_itemList.keys())
    {
        if (idList.indexOf(id) == -1)//Not in can-mount list
        {
            DiskItem * item = m_itemList.take(id);
            m_listWidget->removeWidget(m_listWidget->indexOf(item), true);

            qWarning() << "[DiskMountPlugin] Disk Unmounted:" << id;
        }
    }

    int spacing = DISK_ITEM_MARGIN + DISK_ITEM_SPACING;
    int c = m_listWidget->count() > MAX_VISABLE_COUNT ? MAX_VISABLE_COUNT : m_listWidget->count();
    int w = DISK_ITEM_WIDTH + DISK_ITEM_MARGIN * 2;
    int h = qMax((DISK_ITEM_HEIGHT + spacing) * c - spacing, 0);
    setFixedSize(w, h);
}

DiskContent::~DiskContent()
{

}

void DiskContent::resizeEvent(QResizeEvent *e)
{
    if (e->size() != e->oldSize()) {
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, m_id);
    }
}
