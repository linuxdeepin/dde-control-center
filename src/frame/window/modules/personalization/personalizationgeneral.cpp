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
#include <QGraphicsDropShadowEffect>

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

struct ColorStru {
    int r = 0;
    int g = 0;
    int b = 0;
    bool operator==(const ColorStru b) const
    {
        return (b.r == this->r && b.g == this->g && b.b == this->b);
    }
};

const QList<ColorStru> ACTIVE_COLORST = {
    {216, 49, 108},
    {255, 93, 0},
    {248, 203, 0},
    {35, 196, 0},
    {0, 164, 138},
    {0, 129, 255},
    {60, 2, 215},
    {106, 0, 181},
    {77, 77, 77},
};

const QList<ColorStru> Dark_ACTIVE_COLORST = {
    {192, 16, 78},
    {204, 77, 3},
    {217, 177, 0},
    {31, 168, 1},
    {0, 138, 116},
    {0, 89, 210},
    {67, 40, 255},
    {120, 25, 168},
    {100, 100, 100},
};

PersonalizationGeneral::PersonalizationGeneral(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout())
    , m_wmSwitch(nullptr)
    , m_transparentSlider(nullptr)
    , m_cmbMiniEffect(new ComboxWidget)
    , m_Themes(new PerssonalizationThemeWidget())
    , m_bgWidget(new RingColorWidget)
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

    m_themeType = DGuiApplicationHelper::instance()->themeType();

    //active colors
    //~ contents_path /personalization/General
    QLabel *activeL = new TitleLabel(tr("Accent Color"));
    activeL->setMargin(10);
    m_centralLayout->addWidget(activeL);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setAlignment(Qt::AlignLeft);
    m_bgWidget->setLayout(colorLayout);
    m_bgWidget->setFixedHeight(40);
    colorLayout->setContentsMargins(16, 0, 16, 0);
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    for (ColorStru aColor : (m_themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST : Dark_ACTIVE_COLORST)) {
        QColor color;
        color.setRgb(aColor.r, aColor.g, aColor.b);
        RoundColorWidget *colorItem = new RoundColorWidget(color, this);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17);        // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color);      //阴影的颜色
        effect->setOffset(0,5);
        colorItem->setGraphicsEffect(effect);

        if (m_themeType == DGuiApplicationHelper::ColorType::LightType) {
            colorItem->setAccessibleName(ACTIVE_COLORS[ACTIVE_COLORST.indexOf(aColor)]);
        } else {
            colorItem->setAccessibleName(ACTIVE_COLORS[Dark_ACTIVE_COLORST.indexOf(aColor)]);
        }

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
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
        winEffectVLayout->addSpacing(20);
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
        winEffectVLayout->setContentsMargins(0, 0, 0, 0);
        winEffectVLayout->addSpacing(10);

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
        winEffectVLayout->addSpacing(10);

        m_cmbMiniEffect->setTitle(tr("Window Minimize Effect"));
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
                Q_EMIT requestSetMiniEffect(m_cmbMiniEffect->comboBox()->currentIndex());
        });
    }
    m_centralLayout->addWidget(m_switchWidget);
    m_centralLayout->addStretch(20);
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

void PersonalizationGeneral::updateThemeColors(DGuiApplicationHelper::ColorType type)
{
    int count = 0;
    for (auto item : m_activeColorsList) {
        ColorStru acolor = type == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST[count] : Dark_ACTIVE_COLORST[count];
        count++;
        QColor color;
        color.setRgb(acolor.r, acolor.g, acolor.b);
        item->setColor(color);
        DPalette pa = item->palette();
        pa.setBrush(DPalette::Base, color);
        item->setPalette(pa);
        if (dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())) {
            color.setAlpha(68);
            dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())->setColor(color);
        }
    }
}

void PersonalizationGeneral::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (m_themeType != DGuiApplicationHelper::instance()->themeType()) {
        m_themeType = DGuiApplicationHelper::instance()->themeType();
        updateThemeColors(m_themeType);
    }

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
            m_bgWidget->setSelectedItem(item);
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

    RoundColorWidget* pItem = dynamic_cast<RoundColorWidget*>(sender());
    if(nullptr != pItem)
        m_bgWidget->setSelectedItem(pItem);
    //设置active color
    QString strColor = ACTIVE_COLORS[m_activeColorsList.indexOf(activeColor)];
    qDebug() << Q_FUNC_INFO << " strColor : " << strColor;
    Q_EMIT requestSetActiveColor(strColor);
}

void RingColorWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if(nullptr == m_selectedItem)
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    //draw select circle
    QPen pen;
    pen.setBrush(palette().highlight());
    pen.setWidth(borderWidth);  //pen width
    painter.setPen(pen);
    QRect rc = m_selectedItem->geometry();
    painter.drawEllipse(QRect(rc.center().x() - 14, rc.center().y() - 14, 30, 30));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
}
