/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "titledslideritem.h"

#include <QLabel>
#include <QSlider>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "labels/normallabel.h"
#include "dccslider.h"
#include "dccsliderannotated.h"

namespace dcc {
namespace widgets {

TitledSliderItem::TitledSliderItem(QString title, QWidget *parent) :
    SettingsItem(parent),
    m_titleLabel(new NormalLabel(title)),
    m_valueLabel(new NormalLabel),
    m_slider(new DCCSliderAnnotated),
    m_leftIconLabel(new QLabel),
    m_rightIconLabel(new QLabel)
{
    m_slider->slider()->setOrientation(Qt::Horizontal);
    m_leftIconLabel->setVisible(false);
    m_rightIconLabel->setVisible(false);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_valueLabel);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_leftIconLabel);
    bottomLayout->addWidget(m_slider, 0);
    bottomLayout->addWidget(m_rightIconLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    setFixedHeight(mainLayout->sizeHint().height());
}

DCCSlider *TitledSliderItem::slider() const
{
    return m_slider->slider();
}

void TitledSliderItem::setAnnotations(const QStringList &annotations)
{
    m_slider->setAnnotations(annotations);
}

QString TitledSliderItem::leftIcon() const
{
    return m_leftIcon;
}

void TitledSliderItem::setLeftIcon(const QString &leftIcon)
{
    if (leftIcon != m_leftIcon) {
        m_leftIcon = leftIcon;

        QPixmap pix(m_leftIcon);
        m_leftIconLabel->setPixmap(pix);
        m_leftIconLabel->setVisible(!pix.isNull());

        emit leftIconChanged();
    }
}

QString TitledSliderItem::rightIcon() const
{
    return m_rightIcon;
}

void TitledSliderItem::setRightIcon(const QString &rightIcon)
{
    if (rightIcon != m_rightIcon) {
        m_rightIcon = rightIcon;

        QPixmap pix(m_rightIcon);
        m_rightIconLabel->setPixmap(pix);
        m_rightIconLabel->setVisible(!pix.isNull());

        emit rightIconChanged();
    }
}

QString TitledSliderItem::valueLiteral() const
{
    return m_valueLiteral;
}

void TitledSliderItem::setValueLiteral(const QString &valueLiteral)
{
    if (valueLiteral != m_valueLiteral) {
        m_valueLiteral = valueLiteral;
        m_valueLabel->setText(m_valueLiteral);
    }
}

} // namespace widgets
} // namespace dcc
