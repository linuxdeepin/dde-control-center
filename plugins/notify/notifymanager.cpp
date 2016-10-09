/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifymanager.h"

NotifyManager::NotifyManager(QWidget *parent) : QWidget(parent) {
    m_layout = new QVBoxLayout();
    m_dataSource = new DataSourceThread();
    m_layout->addStretch();
    m_layout->setDirection(QVBoxLayout::BottomToTop);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(1);
    this->setLayout(m_layout);
    m_dataSource->start();
    connect(m_dataSource, &DataSourceThread::ValueChanged, this, &NotifyManager::setValue);
}

NotifyManager::~NotifyManager() {
    m_dataSource->deleteLater();
    m_viewer->deleteLater();
    m_layout->deleteLater();
}

void NotifyManager::setValue(QByteArray s) {
    QString appName,appIcon,summary,body;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(s, &json_error);
    if(json_error.error == QJsonParseError::NoError) {
        if(parse_doucment.isObject()) {
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("appName")) {
                QJsonValue name_value = obj.take("appName");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    appName = name;
                }
            }
            if(obj.contains("appIcon")) {
                QJsonValue name_value = obj.take("appIcon");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    appIcon = name;
                }
            }
            if(obj.contains("summary")) {
                QJsonValue name_value = obj.take("summary");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    summary = name;
                }
            }
            if(obj.contains("body")) {
                QJsonValue name_value = obj.take("body");
                if(name_value.isString()) {
                    QString name = name_value.toString();
                    body = name;
                }
            }
        }
    }
    m_viewer = new Viewer(this);
    m_viewer->setFixedHeight(80);
    m_viewer->setContentsMargins(0, 0, 0, 0);
    m_viewer->setStyleSheet("Viewer {background-color: rgba(255, 255, 255, 0.03);}"
                            "Viewer:hover {background-color: rgba(254, 254, 254, 0.13);}");
    QString m_time=QTime::currentTime().toString("AP hh:mm");
    m_viewer->setValue(appName, appIcon, summary, body, m_time);
    m_layout->addWidget(m_viewer);
}
