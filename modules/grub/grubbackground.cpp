/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QDebug>
#include <QLabel>
#include <QMimeDatabase>

#include "grubbackground.h"
#include "dbustheme.h"

GrubBackground::GrubBackground(GrubThemeDbus *themeDbus, QWidget *parent) :
    QFrame(parent),
    m_themeDbus(themeDbus),
    m_isDrop(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    updateBackground(m_themeDbus->background());
    connect(m_themeDbus, &GrubThemeDbus::backgroundChanged, this, &GrubBackground::updateBackground);

    setAcceptDrops(true);
}

void GrubBackground::paintEvent(QPaintEvent *e)
{
    if(!m_background.isNull()){
        QPainter pa(this);
        pa.drawPixmap(rect(), m_background);
        if(m_isDrop){
            pa.fillRect(rect(), QColor(0, 0, 0, 100));
        }
    }

    QFrame::paintEvent(e);
}

void GrubBackground::dragEnterEvent(QDragEnterEvent *e)
{
    QMimeDatabase b;
    foreach (QUrl url, e->mimeData()->urls()) {
        QString mime = b.mimeTypeForUrl(url).name();

        if(!mime.contains("image/"))
            continue;

        if(e->mimeData()->hasUrls()){
            e->acceptProposedAction();
            m_isDrop = true;
            update();
        }

        return;
    }
}

void GrubBackground::dragLeaveEvent(QDragLeaveEvent*)
{
    m_isDrop = false;
    update();
}

void GrubBackground::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->urls().isEmpty())
        return;

    QString path = e->mimeData()->urls()[0].toLocalFile();
    if(path!=""){
        m_themeDbus->SetBackgroundSourceFile(path);
        e->acceptProposedAction();
        m_isDrop = false;
        update();
    }
}

bool GrubBackground::updateBackground(const QString &filename)
{
    bool result = m_background.load(filename);
    if(result){
        setMinimumSize(m_background.size());
        update();
    }
    return result;
}

