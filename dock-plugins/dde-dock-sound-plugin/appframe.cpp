/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "appframe.h"

const int TITLE_HEIGHT = 35;
const int TITLE_WIDTH = 240;
const int SPLIT_WIDTH = 150;
const int TITLE_LEFT_MARGIN = 20;
const int SLIDER_HEIGHT = 40;
AppFrame::AppFrame(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setLayout(m_mainLayout);
    initTitle();
    initAppList();
    adjustSize();
}

QSize AppFrame::getContentSize()
{
    return QSize(TITLE_WIDTH,
                 TITLE_HEIGHT + m_audio->sinkInputs().count() * SLIDER_HEIGHT);
}

void AppFrame::updateSize()
{
    setFixedSize(getContentSize());
}

void AppFrame::initTitle()
{
    QLabel *titleLabel = new QLabel(tr("Applications"));
    QLabel *splitLabel = new QLabel();
    splitLabel->setObjectName("SplitLine");
    splitLabel->setFixedSize(SPLIT_WIDTH, 1);

    QWidget *titleWidget = new QWidget();
    titleWidget->setFixedSize(TITLE_WIDTH, TITLE_HEIGHT);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(TITLE_LEFT_MARGIN, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignLeft);
    titleLayout->addWidget(splitLabel, 1, Qt::AlignLeft);

    m_mainLayout->addWidget(titleWidget);
}

void AppFrame::initAppList()
{
    m_audio = new DBusAudio(this);
    QList<QDBusObjectPath> pathList = m_audio->sinkInputs();

    if (pathList.count() <= 0)
    {
        setVisible(false);
    }

    for (QDBusObjectPath objPath : pathList)
    {
        QString path = objPath.path();
        AppIconSlider *iconSlider = new AppIconSlider(path, this);
        if (iconSlider->isValid()) {
            iconSlider->setFixedSize(TITLE_WIDTH, SLIDER_HEIGHT);
            m_mainLayout->addWidget(iconSlider, 1 , Qt::AlignLeft);

            if (m_appsMap.keys().indexOf(path) == -1)
                m_appsMap.insert(path, iconSlider);
        }
        else {
            iconSlider->deleteLater();
            break;
        }

    }
    connect(m_audio,&DBusAudio::SinkInputsChanged,this,&AppFrame::updateAppList);
}

void AppFrame::updateAppList()
{
    QList<QDBusObjectPath> pathList = m_audio->sinkInputs();

    emit appCountChanged(pathList.count());

    QStringList tmplist;
    foreach (QDBusObjectPath tmpPath, pathList) {
        if (tmplist.indexOf(tmpPath.path()) == -1)
            tmplist.append(tmpPath.path());
    }

    for (int i = 0; i < tmplist.length(); i ++)
    {
        QString tmpPath = tmplist.at(i);
        if (m_appsMap.keys().indexOf(tmplist.at(i)) == -1)//new path,add to list
        {
            AppIconSlider *iconSlider = new AppIconSlider(tmpPath,this);
            if (iconSlider->isValid())
            {
                iconSlider->setFixedSize(TITLE_WIDTH, SLIDER_HEIGHT);
                m_mainLayout->addWidget(iconSlider);
                m_mainLayout->setAlignment(iconSlider,Qt::AlignRight);
                m_appsMap.insert(tmpPath,iconSlider);
            }
            else
                delete iconSlider;

            return;
        }
    }

    for (int i = 0; i < m_appsMap.count(); i ++)
    {
        if (tmplist.indexOf(m_appsMap.keys().at(i)) == -1)//path has remove
        {
            m_mainLayout->removeWidget(m_appsMap.value(m_appsMap.keys().at(i)));
            AppIconSlider *tmpSlider = m_appsMap.take(m_appsMap.keys().at(i));
            if (tmpSlider)
                delete tmpSlider;
            break;
        }
    }

}
