/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CLOCKPIXMAP_H
#define CLOCKPIXMAP_H

#include <QPixmap>
#include <QTime>
#include <QList>

class ClockPixmap : public QPixmap
{
public:
    ClockPixmap();
    ClockPixmap(QTime time);
    ~ClockPixmap();

    QTime getTime() const;
    void setTime(const QTime &time);

    bool getIn24hour() const;
    void setIn24hour(bool in24hour);

    bool getAnalog() const;
    void setAnalog(bool analog);

private:
    QTime m_time;
    bool m_in24hour;
    bool m_analog;

    void paint();
    QPixmap getNamedPixmap(QString name);
    QPixmap getNumberPixmap(int number, bool big = true);

    QList<int> getTimeTuple();
};

#endif // CLOCKPIXMAP_H
