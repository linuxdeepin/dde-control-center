/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             Chris Xiong <chirs241097@gmail.com>
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

#include "scalingpage.h"
#include "displaymodel.h"
#include "monitor.h"
#include "displaywidget.h"
#include "widgets/dccslider.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/tipslabel.h"

using namespace dcc::widgets;

namespace dcc {

namespace display {

ScalingPage::ScalingPage(QWidget *parent)
    : ContentWidget(parent),
      m_centralLayout(new QVBoxLayout)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);

    QWidget *w = new TranslucentFrame;
    w->setLayout(m_centralLayout);

    m_slidersgrp = new SettingsGroup;
    m_centralLayout->addWidget(m_slidersgrp);

    TipsLabel *tip = new TipsLabel(tr("Some applications in multi-screens may not scale as the settings."), this);
    tip->setWordWrap(true);
    tip->setContentsMargins(16, 5, 10, 5);
    tip->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_centralLayout->addWidget(tip);

    setTitle(tr("Display Scaling"));
    setContent(w);
}

void ScalingPage::setModel(DisplayModel *model)
{
    m_displayModel = model;

    setupSliders();
}

void ScalingPage::setupSliders()
{
    for(auto& s : m_sliders)
        delete s;
    m_sliders.clear();
    for(int i = 0; i < m_displayModel->monitorList().size(); ++i)
        addSlider(i);
}

void ScalingPage::addSlider(int monitorID)
{
    m_sliders.push_back(
        new TitledSliderItem(
            ~monitorID ? tr("Display scaling for %1").arg(m_displayModel->monitorList()[monitorID]->name())
                       : tr("Display scaling for all monitors")
        )
    );
    QStringList scaleList;
    scaleList << "1.0"
              << "1.25"
              << "1.5"
              << "1.75"
              << "2.0"
              << "2.25"
              << "2.5"
              << "2.75"
              << "3.0";
    TitledSliderItem *slideritem=m_sliders.back();
    DCCSlider *slider = slideritem->slider();
    slider->setRange(1, 9);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    slideritem->setAnnotations(scaleList);
    m_slidersgrp->appendItem(slideritem);

    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        Q_EMIT requestIndividualScaling(m_displayModel->monitorList()[monitorID],
                                        DisplayWidget::convertToScale(value));

        slideritem->setValueLiteral(QString::number(DisplayWidget::convertToScale(value)));
    });

    double scaling = m_displayModel->monitorList()[monitorID]->scale();
    if(scaling < 0)scaling = 1.0;
    slider->setValue(DisplayWidget::convertToSlider(scaling));
    slideritem->setValueLiteral(QString::number(scaling));
}

}
}
