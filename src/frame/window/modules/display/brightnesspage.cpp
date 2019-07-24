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

#include "brightnesspage.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/brightnessitem.h"

#include "widgets/labels/tipslabel.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"

using namespace dcc::widgets;
using namespace dcc::display;

namespace DCC_NAMESPACE {

namespace display {

BrightnessPage::BrightnessPage(QWidget *parent)
    : QWidget(parent),
      m_centralLayout(new QVBoxLayout)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);

    setLayout(m_centralLayout);
}

void BrightnessPage::setMode(DisplayModel* model)
{
    m_displayModel = model;

    addSlider();
}

void BrightnessPage::addSlider()
{
    auto monList = m_displayModel->monitorList();

    QString title = tr("Display scaling for all monitors");
    for (int i = 0; i < monList.size(); ++i)
    {
        monList[i]->brightness();

        QStringList scaleList;
        scaleList << "0%"
                  << "20%"
                  << "40%"
                  << "60%"
                  << "80%"
                  << "100%";

        //单独显示每个亮度调节名
        title = i ? title : tr("Display scaling for %1").arg(monList[i]->name());

        TitledSliderItem *slideritem = new TitledSliderItem(title);
        DCCSlider *slider = slideritem->slider();
//        slider->setRange(1, 9);/
        slider->setType(DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(1);
        slider->setPageStep(1);
        slideritem->setAnnotations(scaleList);

        m_centralLayout->addWidget(slideritem);
    }
}

}

}
