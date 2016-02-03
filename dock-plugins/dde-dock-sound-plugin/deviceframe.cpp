/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "deviceframe.h"

const int TITLE_WIDTH = 240;
const int SPLIT_WIDTH = 150;
const int TITLE_LEFT_MARGIN = 20;
const int SLIDER_HEIGHT = 30;
DeviceFrame::DeviceFrame(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    m_audio = new DBusAudio(this);

    setLayout(m_mainLayout);
    initTitle();
    initDevice();
    adjustSize();

    //data will change with default-sink-change
    connect(m_audio, &DBusAudio::DefaultSinkChanged, this, [=] {
        m_mainLayout->removeWidget(m_iconSlider);
        m_iconSlider->deleteLater();
        //audio-backend's data is always delayed loaging
        QTimer::singleShot(1000, this, SLOT(initDevice()));
    });
}


void DeviceFrame::initTitle()
{
    QLabel *titleLabel = new QLabel(tr("Device"));
    QLabel *splitLabel = new QLabel();
    splitLabel->setObjectName("SplitLine");
    splitLabel->setFixedSize(SPLIT_WIDTH, 1);
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(TITLE_LEFT_MARGIN, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignLeft);
    titleLayout->addWidget(splitLabel, 1, Qt::AlignLeft);
    QWidget *titleWidget = new QWidget();
    titleWidget->setLayout(titleLayout);
    titleWidget->setFixedSize(TITLE_WIDTH, 35);

    m_mainLayout->addWidget(titleWidget);
    m_mainLayout->setAlignment(titleWidget,Qt::AlignRight);
}

void DeviceFrame::initDevice()
{
    QString path = QDBusObjectPath(m_audio->GetDefaultSink().value()).path();
    m_iconSlider = new DeviceIconSlider(path,this);
    m_iconSlider->setFixedSize(TITLE_WIDTH, SLIDER_HEIGHT);

    m_mainLayout->addWidget(m_iconSlider, 0, Qt::AlignRight);
}
