/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>
#include <QWidget>
#include "notifydata.h"
#include "notifyviewer.h"
#include <QVBoxLayout>
#include <QTime>

class NotifyManager : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyManager(QWidget *parent = 0);
    ~NotifyManager();

public slots:
    void setValue(QByteArray s);
    void checkNotify();

private:
    NotifyData *m_dataSource;
    Viewer *m_viewer;
    QVBoxLayout *m_layout;
    QLabel *m_emptyNotify;
};

#endif // NOTIFYMANAGER_H
