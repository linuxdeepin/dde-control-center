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
#include <QScrollBar>

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

    QScrollBar *bar = scrollarea->verticalScrollBar();
    connect(bar, &QScrollBar::valueChanged, this, [=](int value){
            if (m_checkIndex && value == bar->maximum())
                onLoadAgain();
    });

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    m_clearButton = new DImageButton;
    m_clearButton->setText(tr("Clear all"));
    m_clearButton->setStyleSheet("padding: 4px 0;");

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
    Viewer* viewer = onNotifyAdd(jsonDocument.object());

    m_connectLayout->insertWidget(0, viewer);
}

Viewer *NotifyManager::onNotifyAdd(const QJsonObject &value) {
    m_clearButton->setVisible(true);

    Viewer* viewer = new Viewer(value, this);

    viewer->setAppName(value["summary"].toString());
    viewer->setAppBody(value["body"].toString());
    viewer->setAppIcon(value["icon"].toString());
    viewer->setAppId(value["id"].toString());

    const QDateTime date = QDateTime::fromMSecsSinceEpoch(value["id"].toString().toLongLong());

    if (QDateTime::currentMSecsSinceEpoch() > value["id"].toString().toLongLong()) {

        const QString hour = date.toString("hh:mm");

        const uint year = date.date().year();
        uint now = QDateTime::currentDateTime().date().year();

        if (now > year)
            viewer->setAppTime(date.toString("yyyy/MM/dd hh:mm"));
        else {
            const uint notify_day = date.date().day();
            now = QDateTime::currentDateTime().date().day();

            const uint month = date.date().month();
            const uint now_month = QDateTime::currentDateTime().date().month();

            if (now_month == month) {

                //contrast day
                const uint time = now - notify_day;

                switch (time) {
                case 0:
                    viewer->setAppTime(hour);
                    break;
                case 1:
                    viewer->setAppTime(tr("Yesterday") + " " + hour);
                    break;
                case 2:
                    viewer->setAppTime(tr("The day before yesterday") + " " + hour);
                    break;
                default:
                    if (time > 7) {
                        viewer->setAppTime(date.toString("MM/dd hh:mm"));
                    } else {
                        viewer->setAppTime(tr("%n day(s) ago", "", time) + " " + hour);
                    }
                    break;
                }
            } else {
                viewer->setAppTime(date.toString("MM/dd hh:mm"));
            }
        }
    } else {
        viewer->setAppTime(date.toString("yyyy/MM/dd hh:mm"));
    }

    viewer->setFixedHeight(80);
    viewer->setFixedWidth(360);
    viewer->setContentsMargins(0, 0, 0, 0);

    m_viewerList.insert(viewer, value);
    connect(viewer, &Viewer::requestClose, this, &NotifyManager::onNotifyRemove);
    update();

    return viewer;
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

void NotifyManager::onLoadAgain()
{
    if (m_checkIndex) {
        for (int i = 0; i < 20; i++) {
            if (--m_checkIndex < 1)
                return;
            Viewer* viewer = onNotifyAdd(m_dataJsonArray.at(m_checkIndex).toObject());
            m_connectLayout->insertWidget(m_viewerList.size(), viewer);
        }
    }
}

void NotifyManager::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(255, 255, 255, 255 * 0.03));

    if (m_viewerList.isEmpty()) {
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

    m_dataJsonArray = jsonDocument.array();

    m_checkIndex = m_dataJsonArray.size();

    if (m_checkIndex) {
        if (m_checkIndex < 20) {
            for (const QJsonValue &value : m_dataJsonArray) {
                Viewer* viewer = onNotifyAdd(value.toObject());
                m_connectLayout->insertWidget(0, viewer);
            }
            m_checkIndex = 0;
        } else {
            for (int i = 20; i > 0; --i) {
                if (m_checkIndex - i < 1)
                    return;
                Viewer* viewer = onNotifyAdd(m_dataJsonArray.at(m_checkIndex - i).toObject());
                m_connectLayout->insertWidget(0, viewer);
            }
            m_checkIndex = m_dataJsonArray.size() - 20;
        }
    }
    w->deleteLater();
}

void NotifyManager::onCloseAllItem()
{
    for (Viewer *viewer : m_viewerList.keys()) {
        viewer->onClose();
    }

    m_viewerList.clear();
    m_checkIndex = 0;
    m_dbus->ClearRecords();
}
