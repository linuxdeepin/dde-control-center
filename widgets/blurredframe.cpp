/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "blurredframe.h"

#include <QWindow>
#include <QX11Info>
#include <QPainter>
#include <QPaintEvent>

#include <xcb/xproto.h>

namespace dcc {
namespace widgets {

static void BlurWindowBackground(const WId windowId, const QRect &region)
{
    xcb_connection_t *connection = QX11Info::connection();
    const char *name = "_NET_WM_DEEPIN_BLUR_REGION";
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection,
                                      0,
                                      strlen(name),
                                      name);

    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection,
                                     cookie,
                                     NULL);
    if (reply) {
        const int data[] = {region.x(), region.y(), region.width(), region.height()};

        xcb_change_property_checked(connection,
                                    XCB_PROP_MODE_REPLACE,
                                    windowId,
                                    reply->atom,
                                    XCB_ATOM_CARDINAL,
                                    32,
                                    4,
                                    data);
        xcb_flush(connection);

        free(reply);
    }
}

BlurredFrame::BlurredFrame(QWidget *parent)
    : QFrame(parent),
      m_radius(0)
{

}

void BlurredFrame::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QColor bgColor("#101010");
    bgColor.setAlphaF(0.5);

    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect(), m_radius, m_radius);

    painter.end();
}

void BlurredFrame::resizeEvent(QResizeEvent *event)
{
    const QSize size(event->size());
    const QRect region(QPoint(0, 0), size);
    BlurWindowBackground(winId(), region);

    QFrame::resizeEvent(event);
}

float BlurredFrame::radius() const
{
    return m_radius;
}

void BlurredFrame::setRadius(float radius)
{
    m_radius = radius;
}

} // namespace widgets
} // namespace dcc
