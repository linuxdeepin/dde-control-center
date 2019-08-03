/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "personalizationgeneral.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/dccslider.h"
#include "perssonalizationthemewidget.h"
#include "roundcolorwidget.h"
#include "dwindowmanagerhelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QColor>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

PersonalizationGeneral::PersonalizationGeneral(QWidget *parent)
    : QWidget(parent)
    , m_transparentSlider(new dcc::widgets::DCCSliderAnnotated())
{
    const QList<QColor> colors = {
        QColor(0xD8, 0x31, 0x6C),
        QColor(0xFF, 0x5D, 0x00),
        QColor(0xF8, 0xCB, 0x00),
        QColor(0x23, 0xC4, 0x00),
        QColor(0x00, 0xA4, 0x8A),
        QColor(0x00, 0x81, 0xFF),
        QColor(0x3C, 0x02, 0xFF),
        QColor(0x6A, 0x00, 0x85),
        QColor(0x4D, 0x4D, 0x4D)
    };
    m_centralLayout = new QVBoxLayout();

    //appearance
    m_centralLayout->addWidget(new QLabel(tr("Appearance")));
    //pictures and types
    m_Themes = new PerssonalizationThemeWidget(this);
    m_Themes->setMainLayout(new QHBoxLayout(), true);
    m_centralLayout->addWidget(m_Themes);

    //active colors
    m_centralLayout->addWidget(new QLabel(tr("Active Colors")));

    QHBoxLayout *colorsLayout = new QHBoxLayout();
    for (int i = 0; i < colors.size(); ++i) {
        RoundColorWidget *colorItem = new RoundColorWidget(colors.at(i), this);
        colorItem->setFixedSize(40, 40);
        colorsLayout->addWidget(colorItem);
    }
    m_centralLayout->addLayout(colorsLayout);

    //transparancy switch
    QHBoxLayout *transparancyLayout = new QHBoxLayout();
    transparancyLayout->addWidget(new QLabel(tr("Transparency"), this));
    m_transparentSlider->slider()->setOrientation(Qt::Horizontal);
    m_transparentSlider->setObjectName("Transparency");

    dcc::widgets::DCCSlider *slider = m_transparentSlider->slider();
    QStringList annotions;
    annotions << "0.1" << "0.2" << "0.4" << "0.5" << "0.65" << "0.8" << "1.0";
    m_transparentSlider->setAnnotations(annotions);
    slider->setRange(0, 6);
    slider->setType(dcc::widgets::DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    transparancyLayout->addWidget(m_transparentSlider);
    m_centralLayout->addLayout(transparancyLayout);

    //sw switch
    QHBoxLayout *swswitchLayout = new QHBoxLayout();
    swswitchLayout->addWidget(new QLabel(tr("Window Effect")));
    m_wmSwitch = new Dtk::Widget::DSwitchButton();
    swswitchLayout->addStretch();
    swswitchLayout->addWidget(m_wmSwitch);
    m_centralLayout->addLayout(swswitchLayout);
    connect(m_wmSwitch, &Dtk::Widget::DSwitchButton::checkedChanged, this,
            &PersonalizationGeneral::requestSwitchWM);
    connect(m_wmSwitch, &Dtk::Widget::DSwitchButton::checkedChanged, this, [this] {
        // reset switch state
        m_wmSwitch->setChecked(m_model->is3DWm());
    });

    setLayout(m_centralLayout);

    connect(m_transparentSlider->slider(), &dcc::widgets::DCCSlider::valueChanged, this,
            &PersonalizationGeneral::requestSetOpacity);
}

void PersonalizationGeneral::setModel(dcc::personalization::PersonalizationModel *model)
{
    m_model = model;
    m_Themes->setModel(model->getWindowModel());

    connect(model, &dcc::personalization::PersonalizationModel::wmChanged, this,
            [this](bool checked) {
        m_wmSwitch->blockSignals(true);
        m_wmSwitch->setChecked(checked);
        m_wmSwitch->blockSignals(false);
    });

    m_wmSwitch->setChecked(model->is3DWm());
    connect(model, &dcc::personalization::PersonalizationModel::onOpacityChanged, this,
            &PersonalizationGeneral::onOpacityChanged);

    onOpacityChanged(model->opacity());
}

void PersonalizationGeneral::onOpacityChanged(std::pair<int, double> value)
{
    m_transparentSlider->slider()->blockSignals(true);
    m_transparentSlider->slider()->setValue(value.first);
    m_transparentSlider->slider()->blockSignals(false);
}
