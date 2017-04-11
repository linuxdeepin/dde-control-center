/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifymanager.h"
#include <QDebug>
#include <QScrollArea>

NotifyManager::NotifyManager(QWidget *parent) :
    QWidget(parent),
    m_dbus(new Notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this))
{
    QWidget *widget = new QWidget;

    m_connectLayout = new QVBoxLayout(widget);
    m_connectLayout->setMargin(0);
    m_connectLayout->setSpacing(1);
    m_connectLayout->addStretch();

    QScrollArea *scrollarea = new QScrollArea;
    scrollarea->setWidget(widget);
    scrollarea->setObjectName("scrollarea");
    scrollarea->setWidgetResizable(true);
    scrollarea->setFocusPolicy(Qt::NoFocus);
    scrollarea->setFrameStyle(QFrame::NoFrame);
    scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollarea->setContentsMargins(0, 0, 0, 0);
    scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setStyleSheet("background-color:transparent;");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    m_clearButton = new DImageButton;
    m_clearButton->setText(tr("Clear all"));

    mainLayout->addWidget(m_clearButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(scrollarea);

    setLayout(mainLayout);

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
    m_viewer = new Viewer(value, this);
    m_viewer->setFixedHeight(80);
    m_viewer->setContentsMargins(0, 0, 0, 0);
    m_viewer->setStyleSheet("Viewer {background: transparent;}"
                            "Viewer:hover {background-color: rgba(254, 254, 254, 0.13);border-radius: 4;}");
    m_connectLayout->insertWidget(0, m_viewer);
    m_viewerList.insert(m_viewer, value);
    connect(m_viewer, &Viewer::requestClose, this, &NotifyManager::onNotifyRemove);
    update();
}

void NotifyManager::onNotifyRemove(const QString &id)
{
    for (auto it(m_viewerList.begin()); it != m_viewerList.end(); ++it) {
        if (id == it.value()["id"].toString()) {
            m_connectLayout->removeWidget(it.key());
            m_viewerList.remove(it.key());
            m_dbus->RemoveRecord(id);
            break;
        }
    }

    update();
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
        viewer->onClose();
    }
}
