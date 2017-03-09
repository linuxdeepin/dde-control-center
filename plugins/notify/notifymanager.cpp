/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifymanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


NotifyManager::NotifyManager(QWidget *parent) :
    QWidget(parent),
    m_dbus(new Notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this))
{
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(1);

    m_clearButton = new DImageButton;
    m_clearButton->setText(tr("Clear all"));

    m_mainLayout->insertWidget(0, m_clearButton, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);

    m_clearButton->setVisible(false);

    connect(m_clearButton, &DImageButton::clicked, this, &NotifyManager::onCloseAllItem);

    connect(m_dbus, &Notifications::RecordAdded, this, &NotifyManager::onNotifyAdded);

    m_dbus->setSync(false);

    QDBusPendingReply<QString> notify = m_dbus->GetAllRecords();
    QDBusPendingCallWatcher *notifyWatcher = new QDBusPendingCallWatcher(notify, this);
    connect(notifyWatcher, &QDBusPendingCallWatcher::finished, this, &NotifyManager::onNotifyGetAllFinished);
}

NotifyManager::~NotifyManager() {

}

void NotifyManager::onNotifyAdded(const QString &value)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(value.toLocal8Bit().data());
    onNotifyAdd(jsonDocument.object());
}

void NotifyManager::onNotifyAdd(const QJsonObject &value) {

    m_clearButton->setVisible(true);
    m_viewer = new Viewer(this);

    m_viewer->setAppName(value["name"].toString());
    m_viewer->setAppBody(value["body"].toString());
    m_viewer->setAppIcon(value["icon"].toString());
    m_viewer->setAppId(value["id"].toString());

    QString m_time=QTime::currentTime().toString("AP hh:mm");
    m_viewer->setAppTime(m_time);
    m_viewer->setFixedHeight(80);
    m_viewer->setContentsMargins(0, 0, 0, 0);
    m_viewer->setStyleSheet("Viewer {background-color: rgba(255, 255, 255, 0.03);}"
                            "Viewer:hover {background-color: rgba(254, 254, 254, 0.13);}");
    m_mainLayout->insertWidget(1, m_viewer);
    m_viewerList.insert(m_viewer, value);
    connect(m_viewer, &Viewer::requestClose, this, &NotifyManager::onNotifyRemove);
    update();
}

void NotifyManager::onNotifyRemove(const QString &id)
{
    QMap<Viewer*, QJsonObject>::iterator list = m_viewerList.begin();
    while (list!= m_viewerList.end()) {
        if (id == list.value()["id"].toString()) {
            m_viewerList.remove(list.key());
            m_dbus->RemoveRecord(id);
            return;
        }
        ++list;
    }
}

void NotifyManager::paintEvent(QPaintEvent *e)
{
    if (m_viewerList.isEmpty()) {
        QPainter painter(this);
        QString s(tr("No system notifications"));
        painter.drawText(rect(), Qt::AlignCenter, s);
        m_clearButton->setVisible(false);
    }

    QWidget::paintEvent(e);
}

void NotifyManager::onNotifyGetAllFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply.value().toLocal8Bit().data());

    QJsonArray jsonArray = jsonDocument.array();

    for (const QJsonValue &value : jsonArray) {
        onNotifyAdd(value.toObject());
    }

    w->deleteLater();
}


void NotifyManager::onCloseAllItem()
{
    for (Viewer *viewer : m_viewerList.keys()) {
        m_mainLayout->removeWidget(viewer);
        viewer->onClose();
        m_viewerList.remove(viewer);
    }

    m_dbus->ClearRecords();

    update();
}
