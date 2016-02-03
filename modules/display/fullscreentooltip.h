/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FULLSCREENTOOLTIP_H
#define FULLSCREENTOOLTIP_H

#include <QFrame>

class MonitorInterface;
class FullScreenTooltip : public QFrame
{
public:
    explicit FullScreenTooltip(MonitorInterface *dbus);

    void showToCenter();
    void showToTopLeft();

protected:
    void paintEvent(QPaintEvent *e);

private:
    MonitorInterface *m_dbusMonitor;
    bool m_showCenter = true;
    QRect m_monitorRect;
};

#endif // FULLSCREENTOOLTIP_H
