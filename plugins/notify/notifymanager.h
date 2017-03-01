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
#include <dimagebutton.h>
#include <QList>

class NotifyManager : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyManager(QWidget *parent = 0);
    ~NotifyManager();

public slots:
    void setValue(QByteArray s);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void onCloseAllItem();

private:
    NotifyData *m_dataSource;
    Viewer *m_viewer;
    QVBoxLayout *m_mainLayout;
    DImageButton *m_clearButton;
    QList<Viewer*> m_viewerList;
};

#endif // NOTIFYMANAGER_H
