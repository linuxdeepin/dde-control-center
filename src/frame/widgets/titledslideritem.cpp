// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/titledslideritem.h"

#include <QLabel>
#include <QSlider>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QTimer>

#include "widgets/labels/normallabel.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/basiclistdelegate.h"

namespace dcc {
namespace widgets {

TitledSliderItem::TitledSliderItem(QString title, QWidget *parent)
    : SettingsItem(parent)
    , m_titleLabel(new NormalLabel(title))
    , m_valueLabel(new NormalLabel)
    , m_slider(new DCCSlider)
    , m_autoBrightness(new DTK_WIDGET_NAMESPACE::DCheckBox(this))
{
    m_slider->qtSlider()->setAccessibleName(title);
    m_sliderPressed = false;
    QObject::connect(m_slider, &DCCSlider::sliderPressed, this, [this] {
        this->m_sliderPressed = true;
    });
    QObject::connect(m_slider, &DCCSlider::sliderReleased, this, [this] {
        this->m_sliderPressed = false;
    });



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

    m_autoBrightness->setText(tr("Auto Brightness"));
    mainLayout->addWidget(m_autoBrightness);
    m_autoBrightness->setVisible(false);

    connect(m_autoBrightness, &QCheckBox::stateChanged, this, &TitledSliderItem::notifyCheckStateChanged);

    setAccessibleName(title);
}

DCCSlider *TitledSliderItem::slider() const
{
    return m_slider->slider();
}

bool TitledSliderItem::isSliderPressed() const
{
    return m_sliderPressed;
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

void TitledSliderItem::setAutoBrightnessVisible(const bool visible)
{
    if (m_autoBrightness && m_autoBrightness->isVisible() != visible) {
        m_autoBrightness->setVisible(visible);
    }
}

void TitledSliderItem::setAutoBrightnessChecked(const bool checked)
{
    if (!m_autoBrightness) {
        return;
    }
    if (m_autoBrightness->isChecked() == checked) {
        return;
    }
    m_autoBrightness->setChecked(checked);
}

} // namespace widgets
} // namespace dcc
