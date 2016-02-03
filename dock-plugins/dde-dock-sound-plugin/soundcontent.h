/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDCONTENT_H
#define SOUNDCONTENT_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>
#include "appframe.h"
#include "deviceframe.h"
#include <dde-dock/dockpluginproxyinterface.h>

class SoundContent : public QWidget
{
    Q_OBJECT
public:
    explicit SoundContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent = 0);

signals:

public slots:

private:
    void initStyleSheet();
    void initFrame();

private:
    QString m_id = "";
    DockPluginProxyInterface * m_proxy = NULL;

};

#endif // SOUNDCONTENT_H
