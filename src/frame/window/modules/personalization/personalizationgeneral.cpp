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
#include "perssonalizationthemewidget.h"
#include "roundcolorwidget.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/settingsitem.h"

#include <DStyle>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QRect>
#include <QPalette>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

const QList<QString> ACTIVE_COLORS = {
    "#D8316C",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#0081FF",
    "#3C02FF",
    "#6A0085",
    "#4D4D4D"
};

PersonalizationGeneral::PersonalizationGeneral(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout())
    , m_wmSwitch(new DTK_WIDGET_NAMESPACE::DSwitchButton())
    , m_transparentSlider(new dcc::widgets::DCCSliderAnnotated())
    , m_Themes(new PerssonalizationThemeWidget())
{
    //appearance
    //~ contents_path /personalization/General
    m_centralLayout->addWidget(new QLabel(tr("Theme")));
    //pictures and types
    m_Themes->setMainLayout(new QHBoxLayout(), true);
    m_centralLayout->addWidget(m_Themes);

    //active colors
    //~ contents_path /personalization/General
    m_centralLayout->addWidget(new QLabel(tr("Accent Color")));

    m_colorLayout = new QHBoxLayout();
    m_colorLayout->setSpacing(0);
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    for (QString aColor : ACTIVE_COLORS) {
        RoundColorWidget *colorItem = new RoundColorWidget(aColor, this);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        m_colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &PersonalizationGeneral::onActiveColorClicked);
        m_activeColorsList.append(colorItem);
    }

    m_centralLayout->addLayout(m_colorLayout);

    //transparancy switch
    QVBoxLayout *transparancyLayout = new QVBoxLayout();
    SettingsItem *transitem = new dcc::widgets::SettingsItem;
    transitem->addBackground();
    transitem->setLayout(transparancyLayout);

    //~ contents_path /personalization/General
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
    m_centralLayout->addWidget(transitem);

    //sw switch
    QHBoxLayout *swswitchLayout = new QHBoxLayout();
    SettingsItem *switem = new dcc::widgets::SettingsItem;
    switem->addBackground();
    switem->setLayout(swswitchLayout);

    //~ contents_path /personalization/General
    swswitchLayout->addWidget(new QLabel(tr("Window Effect")));
    swswitchLayout->addStretch();
    swswitchLayout->addWidget(m_wmSwitch);
    m_centralLayout->addWidget(switem);

    connect(m_wmSwitch, &DTK_WIDGET_NAMESPACE::DSwitchButton::clicked, this,
            &PersonalizationGeneral::requestSwitchWM);

    m_centralLayout->setSpacing(20);
    m_centralLayout->addStretch();
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

    connect(model, &dcc::personalization::PersonalizationModel::onActiveColorChanged, this,
            &PersonalizationGeneral::onActiveColorChanged);
    onActiveColorChanged(model->getActiveColor());
}

void PersonalizationGeneral::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    DStylePainter painter(this);
    if (m_colorLayout == nullptr || !m_colorLayout->geometry().isValid())
        return;

    QRect r = m_colorLayout->geometry();
    int frame_radius = 18;
    QPalette pal = palette();

    painter.setBrush(pal.background());
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(r, frame_radius, frame_radius);
}

void PersonalizationGeneral::updateActiveColors(RoundColorWidget *selectedWidget)
{
    for (RoundColorWidget *item : m_activeColorsList) {
        if (item == selectedWidget) {
            item->setSelected(true);
        } else {
            item->setSelected(false);
        }
    }
}

void PersonalizationGeneral::onOpacityChanged(std::pair<int, double> value)
{
    m_transparentSlider->slider()->blockSignals(true);
    m_transparentSlider->slider()->setValue(value.first);
    m_transparentSlider->slider()->blockSignals(false);
}

void PersonalizationGeneral::onActiveColorChanged(const QString &newColor)
{
    int colorIndex = ACTIVE_COLORS.indexOf(newColor.trimmed());

    if (colorIndex >= 0)
        updateActiveColors(m_activeColorsList[colorIndex]);
}

void PersonalizationGeneral::onActiveColorClicked()
{
    RoundColorWidget *activeColor = qobject_cast<RoundColorWidget *>(sender());

    //设置active color
    QString strColor = ACTIVE_COLORS[m_activeColorsList.indexOf(activeColor)];
    qDebug() << Q_FUNC_INFO << " strColor : " << strColor;
    Q_EMIT requestSetActiveColor(strColor);
}
