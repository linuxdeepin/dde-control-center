/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "widgets/settingsitem.h"

#include "widgets/accessibleinterface.h"

#include <DPalette>
#include <DPaletteHelper>
#include <DStyle>

#include <QPalette>
#include <QResizeEvent>
#include <QStyle>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(SettingsItem, "SettingsItem");

SettingsItem::SettingsItem(QWidget *parent)
    : QFrame(parent)
    , m_isErr(false)
    , m_hasBack(false)
    , m_hover(false)
    , m_clickable(false)
{
}

bool SettingsItem::isErr() const
{
    return m_isErr;
}

void SettingsItem::setIsErr(const bool err)
{
    if (m_isErr == err)
        return;
    m_isErr = err;

    style()->unpolish(this);
    style()->polish(this);
}

void SettingsItem::addBackground()
{
    m_hasBack = true;
    update();
}

void SettingsItem::removeBackground()
{
    m_hasBack = false;
    update();
}

bool SettingsItem::clickable() const
{
    return m_clickable;
}

void SettingsItem::setClickable(const bool clickable)
{
    m_clickable = clickable;
}

void SettingsItem::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
}

void SettingsItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (m_hasBack) {
        const DPalette &dp = DPaletteHelper::instance()->palette(this);
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(dp.brush((m_clickable && m_hover) ? DPalette::ObviousBackground
                                                     : DPalette::ItemBackground));
        p.drawRoundedRect(rect(), 8, 8);
    }
    return QFrame::paintEvent(event);
}

void SettingsItem::enterEvent(QEvent *event)
{
    m_hover = true;
    update();
}

void SettingsItem::leaveEvent(QEvent *event)
{
    m_hover = false;
    update();
}

void SettingsItem::mousePressEvent(QMouseEvent *event)
{
    if (m_clickable)
        Q_EMIT clicked(this);
}
