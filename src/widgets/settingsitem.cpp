//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/settingsitem.h"
#include "widgets/accessibleinterface.h"

#include <DPalette>
#include <DStyle>
#include <DPaletteHelper>

#include <QStyle>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPalette>

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
        p.setBrush(dp.brush((m_clickable && m_hover) ? DPalette::ObviousBackground : DPalette::ItemBackground));
        p.drawRoundedRect(rect(), 8, 8);
    }
    return QFrame::paintEvent(event);
}

void SettingsItem::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hover = true;
    update();
}

void SettingsItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hover = false;
    update();
}

void SettingsItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (m_clickable)
        Q_EMIT clicked(this);
}
