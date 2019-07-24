/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "displaywidget.h"
#include "../../namespace.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent),
      m_rotate(new DImageButton),
      m_multiScreens(new QPushButton(tr("Multi-Screens"))),
      m_customSetting(new QPushButton(tr("Custom Settings"))),
      m_centralLayout(new QVBoxLayout)
{
    setObjectName("Display");

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);


    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_multiScreens = new QPushButton(tr("Multi-Screens"));
    m_centralLayout->addWidget(m_multiScreens);
    m_centralLayout->setAlignment(m_multiScreens, Qt::AlignLeft | Qt::AlignTop);

    setMaximumWidth(500);

    QPushButton *btn{nullptr};
    btn = new QPushButton(tr("Display Scaling"));
    m_centralLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &DisplayWidget::showScalingPage);

    btn = new QPushButton(tr("Resolution"));
    m_centralLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &DisplayWidget::showResolutionPage);

    btn = new QPushButton(tr("Brightness"));
    m_centralLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &DisplayWidget::showBrightnessPage);

    m_centralLayout->addWidget(m_customSetting);

    m_centralLayout->addStretch(1);
    m_rotate->setNormalPic(":/display/themes/dark/icons/rotate_normal.png");
    m_rotate->setHoverPic(":/display/themes/dark/icons/rotate_hover.png");
    m_rotate->setPressPic(":/display/themes/dark/icons/rotate_press.png");

    m_centralLayout->addWidget(m_rotate);
    connect(m_rotate, &DImageButton::clicked, this, &DisplayWidget::requestRotate);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addLayout(m_centralLayout);
    hlayout->addStretch(1);
    setLayout(hlayout);
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onMonitorListChanged);

    onMonitorListChanged();

}

void DisplayWidget::onMonitorListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1) {
        m_multiScreens->hide();
        m_customSetting->hide();
    } else {
        m_multiScreens->show();
        m_customSetting->show();
    }
}

int DisplayWidget::convertToSlider(const float value)
{
    //remove base scale (100), then convert to 1-based value
    //with a stepping of 25
    return (int)round(value * 100 - 100) / 25 + 1;
}

float DisplayWidget::convertToScale(const int value)
{
    return 1.0 + (value - 1) * 0.25;
}
