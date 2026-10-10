// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "dockslider.h"

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

DockSlider::DockSlider(QWidget *parent)
    : DSlider(Qt::Horizontal, parent),
      m_pressed(false),
      m_timer(new QTimer(this))
{
    setPageStep(50);
    m_timer->setInterval(100);
    setMouseWheelEnabled(true);
    connect(m_timer, &QTimer::timeout, this, &DockSlider::onTimeout);
}

void DockSlider::setValue(const int value)
{
    if (m_pressed)
        return;

    blockSignals(true);
    DSlider::setValue(value);
    blockSignals(false);
}

void DockSlider::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (!rect().contains(e->pos()))
            return;
        m_pressed = true;
        DSlider::setValue(maximum() * e->position().x() / rect().width());
    }
}

void DockSlider::mouseMoveEvent(QMouseEvent *e)
{
    const int value = minimum() + (double((maximum()) - minimum()) * e->position().x() / rect().width());
    const int normalized = std::max(std::min(maximum(), value), 0);

    DSlider::setValue(normalized);

    blockSignals(true);
    emit valueChanged(normalized);
    blockSignals(false);
}

void DockSlider::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_pressed = false;
        emit requestPlaySoundEffect();
    }
}

void DockSlider::wheelEvent(QWheelEvent *e)
{
    e->accept();

    m_timer->start();

    DSlider::setValue(value() + (e->angleDelta().y() > 0 ? 2 : -2));
}

void DockSlider::onTimeout()
{
    m_timer->stop();
    emit requestPlaySoundEffect();
}
