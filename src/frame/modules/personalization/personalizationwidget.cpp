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

#include "personalizationwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/dccslider.h"
#include "widgets/nextpagewidget.h"
#include "personalizationmodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "dwindowmanagerhelper.h"

#include <QDebug>
#include <QPushButton>

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

PersonalizationWidget::PersonalizationWidget()
    : ModuleWidget()
    , m_userGroup(new SettingsGroup)
    , m_transparentSlider(new TitledSliderItem(tr("Transparency")))
    , m_trGrp(new SettingsGroup)
{
    setObjectName("Personalization");

    m_trGrp->appendItem(m_transparentSlider);

    m_transparentSlider->setObjectName("Transparency");

    DCCSlider *slider = m_transparentSlider->slider();
    slider->setRange(1, 6);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);

    m_centralLayout->addWidget(m_trGrp);

    m_centralLayout->addWidget(m_userGroup);
    NextPageWidget *theme = new NextPageWidget;
    NextPageWidget *font  = new NextPageWidget;

    m_wmSwitch = new SwitchWidget(tr("Window Effect"));

    theme->setTitle(tr("Theme"));
    font->setTitle(tr("Font"));

    m_userGroup->appendItem(theme);
    m_userGroup->appendItem(font);
    m_userGroup->appendItem(m_wmSwitch);

    setTitle(tr("Personalization"));
    connect(theme, &NextPageWidget::clicked, this,
            &PersonalizationWidget::showThemeWidget);
    connect(font, &NextPageWidget::clicked, this,
            &PersonalizationWidget::showFontsWidget);
    connect(m_wmSwitch, &SwitchWidget::checkedChanged, this,
            &PersonalizationWidget::requestSwitchWM);
    connect(m_wmSwitch, &SwitchWidget::checkedChanged, this, [=] {
        // reset switch state
        m_wmSwitch->setChecked(m_model->is3DWm());
    });

    connect(m_transparentSlider->slider(), &DCCSlider::valueChanged, this,
            &PersonalizationWidget::requestSetOpacity);

    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasBlurWindowChanged, this,
            &PersonalizationWidget::onBlurWindowChanged);

    onBlurWindowChanged();
}

void PersonalizationWidget::setModel(PersonalizationModel *const model)
{
    m_model = model;

    connect(model, &PersonalizationModel::wmChanged, m_wmSwitch,
            &SwitchWidget::setChecked);

    m_wmSwitch->setChecked(model->is3DWm());
    connect(model, &PersonalizationModel::onOpacityChanged, this,
            &PersonalizationWidget::onOpacityChanged);

    onOpacityChanged(model->opacity());
}

void PersonalizationWidget::onOpacityChanged(std::pair<int, double> value)
{
    m_transparentSlider->slider()->blockSignals(true);
    m_transparentSlider->slider()->setValue(value.first);
    m_transparentSlider->slider()->blockSignals(false);
}

void PersonalizationWidget::onBlurWindowChanged()
{
    m_trGrp->setVisible(DWindowManagerHelper::instance()->hasBlurWindow());
}
