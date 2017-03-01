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

NotifyManager::NotifyManager(QWidget *parent) : QWidget(parent) {
    m_dataSource = new NotifyData;

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(1);

    m_clearButton = new DImageButton;
    m_clearButton->setText(tr("Clear all"));

    m_mainLayout->insertWidget(0, m_clearButton, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);

    m_clearButton->setVisible(false);

    connect(m_dataSource, &NotifyData::dataReceived, this, &NotifyManager::setValue);
    connect(m_clearButton, &DImageButton::clicked, this, &NotifyManager::onCloseAllItem);
}

NotifyManager::~NotifyManager() {
    m_dataSource->deleteLater();
}

void NotifyManager::setValue(QByteArray s) {
    m_clearButton->setVisible(true);
    m_viewer = new Viewer(this);
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(s, &json_error);
    if(json_error.error == QJsonParseError::NoError) {
        if(parse_doucment.isObject()) {
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("summary")) {
                QJsonValue name_value = obj.take("summary");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    m_viewer->setAppName(name);
                }
            }
            if(obj.contains("icon")) {
                QJsonValue name_value = obj.take("icon");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    if (name.isEmpty()) {
                        m_viewer->setAppIcon("application-default-icon");
                    } else {
                        m_viewer->setAppIcon(name);
                    }
                }
            }
            if(obj.contains("body")) {
                QJsonValue name_value = obj.take("body");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    m_viewer->setAppBody(name);
                }
            }
        }
    }
    QString m_time=QTime::currentTime().toString("AP hh:mm");
    m_viewer->setAppTime(m_time);
    m_viewer->setFixedHeight(80);
    m_viewer->setContentsMargins(0, 0, 0, 0);
    m_viewer->setStyleSheet("Viewer {background-color: rgba(255, 255, 255, 0.03);}"
                            "Viewer:hover {background-color: rgba(254, 254, 254, 0.13);}");
    m_mainLayout->insertWidget(1, m_viewer);
    m_viewerList.append(m_viewer);
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

void NotifyManager::onCloseAllItem()
{
    for (Viewer *viewer : m_viewerList) {
        m_mainLayout->removeWidget(viewer);
        viewer->onClose();
        m_viewerList.removeOne(viewer);
    }

    update();
}
