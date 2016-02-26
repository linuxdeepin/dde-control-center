/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H

#include <QSlider>

class VolumeSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool isMute READ isMute WRITE setIsMute)
public:
    explicit VolumeSlider(Qt::Orientation orientation, QWidget *parent = 0);
    ~VolumeSlider();

    bool isMute() const;
    void setIsMute(bool value);
protected:
    void mousePressEvent(QMouseEvent *event);

private:
    bool m_isMute = false;
};

#endif // VOLUMESLIDER_H
