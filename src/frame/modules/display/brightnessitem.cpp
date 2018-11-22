/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "brightnessitem.h"
#include "widgets/dccslider.h"

#include <QHBoxLayout>
#include <DHiDPIHelper>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace display {

#define BRIGHTNESS_MUL  1000

BrightnessItem::BrightnessItem(QWidget *parent)
    : SettingsItem(parent),

      m_leftIcon(new QLabel),
      m_rightIcon(new QLabel),
      m_brightnessSlider(new DCCSlider(DCCSlider::Normal))
{
    m_leftIcon->setPixmap(DHiDPIHelper::loadNxPixmap(":/display/themes/dark/icons/light_low.png"));
    m_rightIcon->setPixmap(DHiDPIHelper::loadNxPixmap(":/display/themes/dark/icons/light_high.png"));
    m_brightnessSlider->setOrientation(Qt::Horizontal);
    m_brightnessSlider->setMaximum(BRIGHTNESS_MUL);
    m_brightnessSlider->setMinimum(200);
    m_brightnessSlider->setTracking(true);

    QHBoxLayout *centralLayout = new QHBoxLayout;

    centralLayout->addWidget(m_leftIcon);
    centralLayout->addWidget(m_brightnessSlider);
    centralLayout->addWidget(m_rightIcon);
    centralLayout->setContentsMargins(10, 5, 10, 5);
    centralLayout->setSpacing(5);

    setLayout(centralLayout);

    connect(m_brightnessSlider, &QSlider::valueChanged, [this](const double value) { Q_EMIT requestSetMonitorBrightness(m_monitor, double(value) / BRIGHTNESS_MUL); });
}

void BrightnessItem::setMinimumBrightnessScale(const double scale)
{
    m_brightnessSlider->setMinimum(scale * BRIGHTNESS_MUL);
}

void BrightnessItem::setValue(const double value)
{
    m_brightnessSlider->blockSignals(true);
    m_brightnessSlider->setValue(value * BRIGHTNESS_MUL);
    m_brightnessSlider->blockSignals(false);
}

void BrightnessItem::setMonitor(Monitor * const mon)
{
    m_monitor = mon;
}

}

}
