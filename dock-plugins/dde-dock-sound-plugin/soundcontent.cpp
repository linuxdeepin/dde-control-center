/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundcontent.h"

SoundContent::SoundContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent)
    : QWidget(parent),m_id(id),m_proxy(proxy)
{
    initStyleSheet();
    initFrame();
}


void SoundContent::initStyleSheet()
{
    QFile file("://Resource/qss/default.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else
        qWarning() << "[Error:] Open  style file errr!";
}

void SoundContent::initFrame()
{
    DeviceFrame * deviceFrame = new DeviceFrame();
    deviceFrame->adjustSize();
    AppFrame * appFrame = new AppFrame();
    appFrame->adjustSize();
    connect(appFrame,&AppFrame::appCountChanged,[=](int count){
        qWarning() << "SinkInput Count:" << count;
        if (count <= 0)
            appFrame->hide();
        else
            appFrame->show();
        appFrame->updateSize();

        adjustSize();

        m_proxy->infoChangedEvent(DockPluginInterface::AppletSize, m_id);
    });

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 20, 0, 10);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(deviceFrame);
    mainLayout->addWidget(appFrame);
    mainLayout->setAlignment(deviceFrame,Qt::AlignHCenter);
    mainLayout->setAlignment(appFrame,Qt::AlignHCenter);

    setLayout(mainLayout);
    adjustSize();
}
