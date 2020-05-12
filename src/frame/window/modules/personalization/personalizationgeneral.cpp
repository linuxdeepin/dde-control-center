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
#include "window/utils.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "widgets/settingsitem.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"

#include <DStyle>
#include <DSwitchButton>
#include <DPalette>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QRect>
#include <QPalette>
#include <QSettings>
#include <QComboBox>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

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
    , m_wmSwitch(nullptr)
    , m_transparentSlider(nullptr)
    , m_cmbMiniEffect(new ComboxWidget)
    , m_Themes(new PerssonalizationThemeWidget())
    , m_bgWidget(new QWidget)
    , m_switchWidget(new QWidget)
{
    m_centralLayout->setMargin(0);
    //appearance
    //~ contents_path /personalization/General
    QLabel *themeL = new TitleLabel(tr("Theme"));
    themeL->setMargin(10);
    m_centralLayout->addWidget(themeL);
    //pictures and types
    m_Themes->setMainLayout(new QHBoxLayout(), true);
    m_centralLayout->addWidget(m_Themes);

    //active colors
    //~ contents_path /personalization/General
    QLabel *activeL = new TitleLabel(tr("Accent Color"));
    activeL->setMargin(10);
    m_centralLayout->addWidget(activeL);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    m_bgWidget->setLayout(colorLayout);
    m_bgWidget->setFixedHeight(40);
    colorLayout->setContentsMargins(16, 0, 16, 0);
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    for (QString aColor : ACTIVE_COLORS) {
        RoundColorWidget *colorItem = new RoundColorWidget(aColor, this);
        colorItem->setAccessibleName(aColor);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &PersonalizationGeneral::onActiveColorClicked);
        m_activeColorsList.append(colorItem);
    }

    m_centralLayout->addWidget(m_bgWidget);

    m_bSystemIsServer = IsServerSystem;

    if (!m_bSystemIsServer) {
        //sw switch
        QVBoxLayout *winEffectVLayout = new QVBoxLayout();
        m_switchWidget->setLayout(winEffectVLayout);

        m_wmSwitch = new DSwitchButton();
        m_wmSwitch->setAccessibleName("switchbutton");
        QHBoxLayout *swswitchLayout = new QHBoxLayout();
        SettingsItem *switem = new dcc::widgets::SettingsItem;
        switem->addBackground();
        switem->setLayout(swswitchLayout);

        //~ contents_path /personalization/General
        swswitchLayout->addWidget(new QLabel(tr("Window Effect")));
        swswitchLayout->addStretch();
        swswitchLayout->addWidget(m_wmSwitch);
        winEffectVLayout->addWidget(switem);
        switem->setVisible(false);

        //~ contents_path /personalization/General
        m_transparentSlider = new dcc::widgets::TitledSliderItem(tr("Transparency"));
        //transparancy switch
        m_transparentSlider->addBackground();
        m_transparentSlider->slider()->setOrientation(Qt::Horizontal);
        m_transparentSlider->setObjectName("Transparency");

        //设计效果图变更：增加左右图标
        m_transparentSlider->slider()->setLeftIcon(QIcon::fromTheme("dcc_transparency_low"));
        m_transparentSlider->slider()->setRightIcon(QIcon::fromTheme("dcc_transparency_high"));
        m_transparentSlider->slider()->setIconSize(QSize(24, 24));
        dcc::widgets::DCCSlider *slider = m_transparentSlider->slider();
        slider->setAccessibleName("transparency");
        //设计效果图变更：去掉刻度数字显示
    //    QStringList annotions;
    //    annotions << "0.1" << "0.2" << "0.4" << "0.5" << "0.65" << "0.8" << "1.0";
    //    m_transparentSlider->setAnnotations(annotions);
    //    slider->setRange(0, 6);
        slider->setRange(0, 100);
        slider->setType(dcc::widgets::DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(1);
        slider->setPageStep(1);
        winEffectVLayout->addWidget(m_transparentSlider);

        m_cmbMiniEffect->setTitle(tr("Window Miniminze Effect"));
        m_cmbMiniEffect->addBackground();
        QStringList options;
        options << tr("Scale") << tr("Magic Lamp");
        m_cmbMiniEffect->setComboxOption(options);
        winEffectVLayout->addWidget(m_cmbMiniEffect);

        connect(m_transparentSlider->slider(), &dcc::widgets::DCCSlider::valueChanged, this,
                &PersonalizationGeneral::requestSetOpacity);
        connect(m_transparentSlider->slider(), &dcc::widgets::DCCSlider::sliderMoved, this,
                &PersonalizationGeneral::requestSetOpacity);
        connect(m_cmbMiniEffect, &dcc::widgets::ComboxWidget::onIndexChanged, this,
                &PersonalizationGeneral::requestSetMiniEffect);

        connect(m_wmSwitch, &DTK_WIDGET_NAMESPACE::DSwitchButton::clicked, this, [this](bool checked) {
                qDebug() << "DSwitchButton::clicked:" << checked << ",m_model->is3DWm():" << m_model->is3DWm();
                m_wmSwitch->setChecked(m_model->is3DWm());
                Q_EMIT requestWindowSwitchWM(checked);
        });
    }
    m_centralLayout->addWidget(m_switchWidget);
    m_centralLayout->setSpacing(20);
    m_centralLayout->addStretch();
    setLayout(m_centralLayout);
}

void PersonalizationGeneral::setModel(dcc::personalization::PersonalizationModel *model)
{
    m_model = model;
    m_Themes->setModel(model->getWindowModel());

    if (!m_bSystemIsServer) {
        connect(model, &dcc::personalization::PersonalizationModel::wmChanged, this,
                [this](bool checked) {
            m_wmSwitch->blockSignals(true);
            updateWMSwitcher(checked);
            m_wmSwitch->blockSignals(false);
        });

        updateWMSwitcher(model->is3DWm());
        connect(model, &dcc::personalization::PersonalizationModel::onOpacityChanged, this,
                &PersonalizationGeneral::onOpacityChanged);

        onOpacityChanged(model->opacity());
        connect(model, &dcc::personalization::PersonalizationModel::onMiniEffectChanged, this,
                &PersonalizationGeneral::onMiniEffectChanged);
        onMiniEffectChanged(model->miniEffect());
    }

    connect(model, &dcc::personalization::PersonalizationModel::onActiveColorChanged, this,
            &PersonalizationGeneral::onActiveColorChanged);
    onActiveColorChanged(model->getActiveColor());

//    connect(m_model, &dcc::personalization::PersonalizationModel::onCompositingAllowSwitch, this,
//            &PersonalizationGeneral::onCompositingAllowSwitchChanged);
//    onCompositingAllowSwitchChanged(m_model->getAllowSwitch());

    connect(m_model, &dcc::personalization::PersonalizationModel::onCompositingEnable, this,
            &PersonalizationGeneral::onCompositingEnableChanged);
    onCompositingEnableChanged(m_model->getCompositingEnable());
}

void PersonalizationGeneral::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    DStylePainter painter(this);

    QRect r = m_bgWidget->geometry();
    int frame_radius = 18;

    QPainterPath path;
    path.addRoundedRect(r, frame_radius, frame_radius);
    const DPalette &pal = DApplicationHelper::instance()->palette(this);
    painter.fillPath(path, pal.brush(DPalette::ItemBackground));
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

void PersonalizationGeneral::updateWMSwitcher(bool checked)
{
    if (m_wmSwitch) {
        m_wmSwitch->setChecked(m_model->is3DWm());
    }
    if (m_transparentSlider) {
        m_transparentSlider->setVisible(checked);
        m_cmbMiniEffect->setVisible(checked);
    }
}

void PersonalizationGeneral::onCompositingAllowSwitchChanged(bool value)
{
    if (!m_bSystemIsServer && value) {
        m_switchWidget->setVisible(false);
    } else {
        m_switchWidget->setVisible(true);
    }
}

void PersonalizationGeneral::onCompositingEnableChanged(bool value)
{
    if (!m_bSystemIsServer && value) {
       m_switchWidget->setVisible(true);
    } else {
        m_switchWidget->setVisible(false);
    }
}

void PersonalizationGeneral::onOpacityChanged(std::pair<int, double> value)
{
    if (m_transparentSlider) {
        m_transparentSlider->slider()->blockSignals(true);
        m_transparentSlider->slider()->setValue(value.first);
        m_transparentSlider->setValueLiteral(QString::number(value.second));
        m_transparentSlider->slider()->blockSignals(false);
    }
}

void PersonalizationGeneral::onMiniEffectChanged(int index)
{
    if(index < m_cmbMiniEffect->comboBox()->count())
        m_cmbMiniEffect->comboBox()->setCurrentIndex(index);
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
