// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "onlineicon.h"

#include <QStyle>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

using namespace DCC_NAMESPACE::accounts;

OnlineIcon::OnlineIcon(QWidget *parent)
    : QWidget(parent)
    , m_shadowEffect(new QGraphicsDropShadowEffect(this))
{
    m_shadowEffect->setColor(QColor(0, 0, 0, 16));      // 阴影的颜色
    m_shadowEffect->setOffset(0, 2);
    setGraphicsEffect(m_shadowEffect);
}

void OnlineIcon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(QBrush(QColor(103, 239, 74)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    setGraphicsEffect(m_shadowEffect);
    painter.restore();
}
