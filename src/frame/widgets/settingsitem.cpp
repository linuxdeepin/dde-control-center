// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

