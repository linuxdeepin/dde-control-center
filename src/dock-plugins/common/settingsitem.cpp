// SPDX-FileCopyrightText: 2016 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "settingsitem.h"

#include <DPalette>
#include <DStyle>
#include <DPaletteHelper>
#include <DGuiApplicationHelper>

#include <QStyle>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPalette>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

SettingsItem::SettingsItem(QWidget *parent)
    : QFrame(parent)
    , m_hasBack(false)
{
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

void SettingsItem::paintEvent(QPaintEvent *event)
{
    if (m_hasBack) {
        QPainter p(this);
        p.setPen(Qt::NoPen);
        QColor bgColor;
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
            bgColor = QColor(0, 0, 0, 255 * 0.05);
        } else {
            bgColor = QColor(255, 255, 255, 255 * 0.05);
        }
        p.setBrush(bgColor);
        p.drawRoundedRect(rect(), 8, 8);
    }
    return QFrame::paintEvent(event);
}
