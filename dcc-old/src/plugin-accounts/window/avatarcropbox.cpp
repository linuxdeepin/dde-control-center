// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "avatarcropbox.h"

#include <QPainter>

#define CropBoxSize 120

using namespace DCC_NAMESPACE;

AvatarCropBox::AvatarCropBox(QWidget *parent)
    : QWidget(parent)
    , m_backgroundColor(parent->palette().color(QPalette::Window))
{
    setFixedSize(190, 190);
}

AvatarCropBox::~AvatarCropBox() { }

void AvatarCropBox::setBackgroundColor(QColor color)
{
    m_backgroundColor = color;

    repaint();
}

void AvatarCropBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto rect = parentWidget()->rect();

    QPainterPath border, cropBox;

    border.setFillRule(Qt::WindingFill);
    border.addRect(rect);

    cropBox.setFillRule(Qt::WindingFill);
    auto adjustSize = (rect.width() - CropBoxSize) / 2;
    cropBox.addRoundedRect(rect.adjusted(adjustSize, adjustSize, -adjustSize, -adjustSize), 10, 10);

    QPainterPath endPath = border.subtracted(cropBox);
    p.fillPath(endPath, m_backgroundColor);
}
