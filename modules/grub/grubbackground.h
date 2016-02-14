/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GRUBBACKGROUND_H
#define GRUBBACKGROUND_H

#include <QFrame>
#include <QPixmap>

class GrubThemeDbus;
class QGraphicsBlurEffect;
class GrubBackground : public QFrame
{
    Q_OBJECT
public:
    explicit GrubBackground(GrubThemeDbus *themeDbus, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *e);

private:
    QPixmap m_background;
    GrubThemeDbus *m_themeDbus;
    bool m_isDrop;

    Q_SLOT bool updateBackground(const QString &filename);
};

#endif // GRUBBACKGROUND_H
