// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/titledslideritem.h"

#include "widgets/accessibleinterface.h"
#include "widgets/dccslider.h"

#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(TitledSliderItem, "TitledSliderItem");

TitledSliderItem::TitledSliderItem(QString title, QWidget *parent)
    : SettingsItem(parent)
    , m_titleLabel(new QLabel(title))
    , m_valueLabel(new QLabel)
    , m_slider(new DCCSlider)
{
    m_slider->qtSlider()->setAccessibleName(title);

    QMargins zeroMg(8, 8, 8, 8);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(zeroMg);
    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_valueLabel);
    topLayout->setMargin(0);
    topLayout->setSpacing(0);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setContentsMargins(zeroMg);
    bottomLayout->addWidget(m_slider, 0);
    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    m_bottomLayout = bottomLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(bottomLayout);

    setAccessibleName(title);
}

DCCSlider *TitledSliderItem::slider() const
{
    return m_slider->slider();
}

void TitledSliderItem::setAnnotations(const QStringList &annotations)
{
    m_slider->setAnnotations(annotations);
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

QString TitledSliderItem::title() const
{
    return m_titleLabel->text();
}

void TitledSliderItem::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitledSliderItem::setLeftIcon(const QIcon &leftIcon)
{
    m_slider->setLeftIcon(leftIcon);
}

void TitledSliderItem::setRightIcon(const QIcon &rightIcon)
{
    m_slider->setRightIcon(rightIcon);
}

void TitledSliderItem::setIconSize(const QSize &size)
{
    m_slider->setIconSize(size);
}
