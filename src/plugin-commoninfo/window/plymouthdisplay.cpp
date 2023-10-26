//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "plymouthdisplay.h"

#include <qpainterpath.h>

#include <QPaintEvent>
#include <QPainter>

using namespace DCC_NAMESPACE;

static constexpr int ItemHeight = 200;
static constexpr qreal Radius = 8.0;

PlyMouthDisplayItem::PlyMouthDisplayItem(QWidget *parent)
    : QWidget(parent)
    , m_logo(QPixmap())
{
    setMinimumHeight(ItemHeight);
}

void PlyMouthDisplayItem::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(rect(), Radius, Radius);
    painter.fillPath(path, Qt::black);

    if (m_logo.isNull()) {
        return;
    }
    qreal x = (rect().width() - m_logo.width()) / 2.0;
    qreal y = (rect().height() - m_logo.height()) / 2.0;
    painter.drawPixmap(x, y, m_logo);
}

void PlyMouthDisplayItem::setLogoPixmap(const QPixmap &pix)
{
    m_logo = pix;
    update();
}
