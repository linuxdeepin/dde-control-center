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

} // namespace widgets
} // namespace dcc
