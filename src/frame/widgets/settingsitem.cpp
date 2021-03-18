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

#include <DPalette>
#include <DStyle>
#include <DApplicationHelper>

#include <QStyle>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPalette>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

namespace dcc {
namespace widgets {

SettingsItem::SettingsItem(QWidget *parent)
    : QFrame(parent)
    , m_isErr(false)
    , m_hasBack(false)
{
}

bool SettingsItem::isErr() const
{
    return m_isErr;
}

void SettingsItem::setIsErr(const bool err)
{
    if (m_isErr == err) return;
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

void SettingsItem::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
}

void SettingsItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (m_hasBack) {
        const DPalette &dp = DApplicationHelper::instance()->palette(this);
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(dp.brush(DPalette::ItemBackground));
        p.drawRoundedRect(rect(), 8, 8);
    }
    return QFrame::paintEvent(event);
}
}
}

