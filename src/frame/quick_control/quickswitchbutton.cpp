/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quickswitchbutton.h"
#include "widgets/basiclistdelegate.h"

#include <QDebug>
#include <QPainter>

namespace dcc {

#define WIDTH 70
#define HEIGHT 60

QuickSwitchButton::QuickSwitchButton(const int index, const QString &iconName, QWidget *parent)
    : QLabel(parent),

      m_index(index),
      m_selected(false),
      m_checked(false),
      m_checkable(true),
      m_showBackground(true),
      m_iconName(iconName)
{
    setFixedSize(WIDTH, HEIGHT);
    setPixmap(normalPixmap());
    setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    setContentsMargins(0, 0, 0, 20);
}

void QuickSwitchButton::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);

    emit clicked(m_index);

    if (m_checkable)
    {
        setChecked(!m_checked);
        emit checkedChanged(m_checked);
    }
}

void QuickSwitchButton::enterEvent(QEvent *e)
{
    QLabel::enterEvent(e);

    emit hovered(m_index);
}

void QuickSwitchButton::paintEvent(QPaintEvent *e)
{
    if (m_selected && m_showBackground)
    {
        const QRect r = rect();

        const int radius = 6;
        const int margin_bottom = 5;

        QPainterPath path;
        path.moveTo(0, 0);
        path.arcTo(QRectF(QPointF(-radius, 0), QPointF(radius, radius * 2)), 90, -90);
        path.lineTo(radius, r.bottom() - margin_bottom - radius * 2);
        path.arcTo(QRectF(QPointF(radius, r.bottom() - margin_bottom - radius * 2), QPointF(radius * 3, r.bottom() - margin_bottom)), 180, 90);
        path.lineTo(r.right() - radius * 2, r.bottom() - margin_bottom);
        path.arcTo(QRectF(QPointF(r.right() - radius * 3, r.bottom() - margin_bottom - radius * 2), QPointF(r.right() - radius, r.bottom() - margin_bottom)), 270, 90);
        path.lineTo(r.right() - radius, radius);
        path.arcTo(QRectF(QPointF(r.right() - radius, 0), QPointF(r.right() + radius, radius * 2)), 180, -90);
        path.closeSubpath();

        QPainter painter(this);
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(255, 255, 255, 255 * .2));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPath(path);
    }

    QLabel::paintEvent(e);
}

QPixmap QuickSwitchButton::normalPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return loadPixmap(QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr));
}

QPixmap QuickSwitchButton::hoverPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr);
}

QPixmap QuickSwitchButton::pressedPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr);
}

QString QuickSwitchButton::themeName() const
{
    return m_themeName;
}

void QuickSwitchButton::setThemeName(const QString &themeName)
{
    m_themeName = themeName;
    setPixmap(normalPixmap());
}

void QuickSwitchButton::setChecked(const bool checked)
{
    m_checked = checked;
    setPixmap(normalPixmap());
}

void QuickSwitchButton::setCheckable(const bool checkable)
{
    m_checkable = checkable;
}

void QuickSwitchButton::setSelected(const bool selected)
{
    m_selected = selected;

    update();
}

void QuickSwitchButton::setBackgroundVisible(const bool visible)
{
    m_showBackground = visible;

    update();
}

}
