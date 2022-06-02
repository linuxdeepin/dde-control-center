/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "personalizationgeneralmodule.h"
#include "widgets/personalizationthemewidget.h"
#include "widgets/roundcolorwidget.h"

#include "widgets/widgetmodule.h"
#include "widgets/titlelabel.h"
#include "widgets/dccslider.h"
#include "personalizationmodel.h"

#include <DFontSizeManager>
#include <DPaletteHelper>
#include <DGuiApplicationHelper>
#include <DSysInfo>
#include <DSwitchButton>
#include <personalizationworker.h>

#include <QGraphicsDropShadowEffect>

#include <widgets/comboxwidget.h>
#include <widgets/settingsitem.h>
#include <widgets/titledslideritem.h>
#include <widgets/switchwidget.h>
//#include <com_deepin_daemon_display.h>
DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const QList<QString> ACTIVE_COLORS = {
    "#D8316C",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#0081FF",
    "#3C02FF",
    "#8C00D4",
    "#4D4D4D"
};

struct ColorStru
{
    int r = 0;
    int g = 0;
    int b = 0;
    bool operator==(const ColorStru b) const
    {
        return (b.r == this->r && b.g == this->g && b.b == this->b);
    }
};

const QList<ColorStru> ACTIVE_COLORST = {
    { 216, 49, 108 },
    { 255, 93, 0 },
    { 248, 203, 0 },
    { 35, 196, 0 },
    { 0, 164, 138 },
    { 0, 129, 255 },
    { 60, 2, 215 },
    { 140, 0, 212 },
    { 77, 77, 77 },
};

const QList<ColorStru> Dark_ACTIVE_COLORST = {
    { 179, 33, 87 },
    { 204, 71, 0 },
    { 198, 162, 0 },
    { 28, 156, 0 },
    { 0, 130, 108 },
    { 0, 105, 204 },
    { 44, 0, 206 },
    { 109, 0, 168 },
    { 61, 61, 61 },
};

void RingColorWidget::paintEvent(QPaintEvent *event)
{
    const DPalette &dp = DPaletteHelper::instance()->palette(this);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(dp.brush(DPalette::ItemBackground));
    painter.drawRoundedRect(rect(), 18, 18);

    QWidget::paintEvent(event);
    if (nullptr == m_selectedItem)
        return;

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    //draw select circle
    QPen pen;
    pen.setBrush(palette().highlight());
    pen.setWidth(borderWidth); //pen width
    painter.setPen(pen);
    QRect rc = m_selectedItem->geometry();
    painter.drawEllipse(QRect(rc.center().x() - 14, rc.center().y() - 14, 30, 30));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
}
////////////////////////////////////////////////////////
PersonalizationGeneralModule::PersonalizationGeneralModule(PersonalizationModel *model, PersonalizationWorker *work, QObject *parent)
    : ModuleObject("personalizationGeneral", tr("General"), QIcon::fromTheme("dcc_common"), parent)
    , m_model(model)
    , m_work(work)
{
    setChildType(ModuleObject::Page);
    m_modules.append(new WidgetModule<TitleLabel>("personalizationThemeTitle", tr("Theme"), [](TitleLabel *title) {
        DFontSizeManager::instance()->bind(title, DFontSizeManager::T5, QFont::DemiBold);
        title->setText(tr("Theme"));
    }));
    m_modules.append(new WidgetModule<PersonalizationThemeWidget>("perssonalGeneralThemes", tr("Theme"), [this](PersonalizationThemeWidget *themes) {
        themes->setMainLayout(new QHBoxLayout(), true);
        themes->setModel(m_model->getWindowModel());
        connect(themes, &PersonalizationThemeWidget::requestSetDefault, m_work, &PersonalizationWorker::setDefault);
    }));
    m_modules.append(new WidgetModule<TitleLabel>("personalizationAccentColorTitle", tr("Accent Color"), [](TitleLabel *title) {
        DFontSizeManager::instance()->bind(title, DFontSizeManager::T5, QFont::DemiBold);
        title->setText(tr("Accent Color"));
    }));
    m_modules.append(new WidgetModule<RingColorWidget>("personalizationAccentColor", tr("Accent Color"), this, &PersonalizationGeneralModule::initAccentColor));

    if (DSysInfo::UosServer != DSysInfo::uosType()) { // 以下服务器版没有
        m_modules.append(new WidgetModule<QWidget>("", "", [](QWidget *w) { w->setFixedHeight(10); }));
        if (!qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland"))
            m_modules.append(new WidgetModule<SwitchWidget>("personalizationWindowEffect", tr("Window Effect"), this, &PersonalizationGeneralModule::initWindowEffect));
        m_modules.append(new WidgetModule<TitledSliderItem>("personalizationTransparencyEffect", tr("Transparency"), this, &PersonalizationGeneralModule::initTransparentEffect));
        m_modules.append(new WidgetModule<ComboxWidget>("personalizationMinimizeEffect", tr("Window Minimize Effect"), this, &PersonalizationGeneralModule::initMiniEffect));
        if (Dtk::Core::DSysInfo::isCommunityEdition()) {
            m_modules.append(new WidgetModule<TitledSliderItem>("personalizationRoundedEffect", tr("Rounded Corner"), this, &PersonalizationGeneralModule::initRoundEffect));
        }
    }
    connect(m_model, &PersonalizationModel::wmChanged, this, &PersonalizationGeneralModule::updateVisible);
    updateVisible(m_model->is3DWm());
}

void PersonalizationGeneralModule::initAccentColor(RingColorWidget *bgWidget)
{
    bgWidget->setFixedHeight(40);
    QHBoxLayout *colorLayout = new QHBoxLayout(bgWidget);
    colorLayout->setAlignment(Qt::AlignLeft);
    colorLayout->setContentsMargins(10, 0, 10, 0);
    int borderWidth = bgWidget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, bgWidget);
    int borderSpacing = bgWidget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, bgWidget);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    for (ColorStru aColor : (themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST : Dark_ACTIVE_COLORST)) {
        QColor color;
        color.setRgb(aColor.r, aColor.g, aColor.b);
        RoundColorWidget *colorItem = new RoundColorWidget(color, bgWidget);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17); // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color); //阴影的颜色
        effect->setOffset(0, 5);
        colorItem->setGraphicsEffect(effect);

        if (themeType == DGuiApplicationHelper::ColorType::LightType) {
            colorItem->setAccessibleName(ACTIVE_COLORS[ACTIVE_COLORST.indexOf(aColor)]);
        } else {
            colorItem->setAccessibleName(ACTIVE_COLORS[Dark_ACTIVE_COLORST.indexOf(aColor)]);
        }

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &PersonalizationGeneralModule::onActiveColorClicked);
    }
    auto setColorFun = [bgWidget](const QString &newColor) {
        QLayout *lyt = bgWidget->layout();
        for (int i = lyt->count() - 1; i >= 0; --i) {
            if (lyt->itemAt(i)->widget()->accessibleName() == newColor) {
                bgWidget->setSelectedItem(qobject_cast<RoundColorWidget *>(lyt->itemAt(i)->widget()));
            }
        }
    };
    setColorFun(m_model->getActiveColor());
    connect(m_model, &PersonalizationModel::onActiveColorChanged, bgWidget, setColorFun);
}

void PersonalizationGeneralModule::initWindowEffect(SwitchWidget *wmSwitch)
{
    wmSwitch->addBackground();
    wmSwitch->setAccessibleName("switchbutton");
    wmSwitch->setTitle(tr("Window Effect"));
    wmSwitch->setChecked(m_model->is3DWm());
    connect(m_model, &PersonalizationModel::wmChanged, wmSwitch, &SwitchWidget::setChecked);
    connect(wmSwitch, &SwitchWidget::checkedChanged, this, [this, wmSwitch](bool checked) {
        qInfo() << "DSwitchButton::clicked:" << checked << ",m_model->is3DWm():" << m_model->is3DWm();
        wmSwitch->setChecked(m_model->is3DWm());
        m_work->windowSwitchWM(checked);
        m_work->setMiniEffect(m_model->miniEffect());
    });
}

void PersonalizationGeneralModule::initTransparentEffect(TitledSliderItem *transparentSlider)
{
    transparentSlider->setTitle(tr("Transparency"));
    transparentSlider->addBackground();
    transparentSlider->slider()->setOrientation(Qt::Horizontal);
    transparentSlider->setObjectName("Transparency");

    //设计效果图变更：增加左右图标
    transparentSlider->slider()->setLeftIcon(QIcon::fromTheme("dcc_transparency_low"));
    transparentSlider->slider()->setRightIcon(QIcon::fromTheme("dcc_transparency_high"));
    transparentSlider->slider()->setIconSize(QSize(24, 24));
    DCCSlider *slider = transparentSlider->slider();
    slider->setAccessibleName("transparency");
    slider->setRange(0, 100);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    auto onOpacityChanged = [transparentSlider](std::pair<int, double> value) {
        transparentSlider->slider()->blockSignals(true);
        transparentSlider->slider()->setValue(value.first);
        transparentSlider->setValueLiteral(QString::number(value.second));
        transparentSlider->slider()->blockSignals(false);
    };
    onOpacityChanged(m_model->opacity());
    connect(m_model, &PersonalizationModel::onOpacityChanged, transparentSlider, onOpacityChanged);
    connect(transparentSlider->slider(), &DCCSlider::valueChanged, m_work, &PersonalizationWorker::setOpacity);
    connect(transparentSlider->slider(), &DCCSlider::sliderMoved, m_work, &PersonalizationWorker::setOpacity);
}

void PersonalizationGeneralModule::initMiniEffect(ComboxWidget *cmbMiniEffect)
{
    cmbMiniEffect->setTitle(tr("Window Minimize Effect"));
    cmbMiniEffect->addBackground();
    QStringList options;
    options << tr("Scale") << tr("Magic Lamp");
    cmbMiniEffect->setComboxOption(options);
    cmbMiniEffect->comboBox()->setCurrentIndex(m_model->miniEffect());
    connect(cmbMiniEffect, &ComboxWidget::onIndexChanged, m_work, &PersonalizationWorker::setMiniEffect);
    connect(m_model, &PersonalizationModel::onMiniEffectChanged, cmbMiniEffect->comboBox(), &QComboBox::setCurrentIndex);
}

void PersonalizationGeneralModule::initRoundEffect(TitledSliderItem *winRoundSlider)
{
    winRoundSlider->setTitle(tr("Rounded Corner"));
    winRoundSlider->addBackground();
    winRoundSlider->slider()->setOrientation(Qt::Horizontal);
    winRoundSlider->setObjectName("winRoundSlider");

    DCCSlider *sliderRound = winRoundSlider->slider();
    QStringList list;
    list << tr("Small") << tr("Medium") << tr("Large");
    sliderRound->setAnnotations(list);
    sliderRound->setType(DCCSlider::Vernier);
    sliderRound->setTickPosition(QSlider::TicksBelow);
    sliderRound->setRange(0, 2);
    sliderRound->setTickInterval(1);
    sliderRound->setPageStep(1);
    auto onWindowRadiusChanged = [winRoundSlider](int radius) {
        if (radius <= 0) {
            winRoundSlider->slider()->setValue(0);
        } else if (radius <= 8) {
            winRoundSlider->slider()->setValue(1);
        } else {
            winRoundSlider->slider()->setValue(2);
        }
    };
    onWindowRadiusChanged(m_model->windowRadius());
    connect(m_model, &PersonalizationModel::onWindowRadiusChanged, sliderRound, onWindowRadiusChanged);
    connect(winRoundSlider->slider(), &DCCSlider::valueChanged, this, [=](int value) {
        int val = value;
        if (value == 0) {
            val = 0;
        } else if (value == 1) {
            val = 8;
        } else if (value == 2) {
            val = 18;
        }
        m_work->setWindowRadius(val);
    });
}

void PersonalizationGeneralModule::onActiveColorClicked()
{
    RoundColorWidget *pItem = dynamic_cast<RoundColorWidget *>(sender());
    //设置active color
    QString strColor = pItem->accessibleName();
    m_work->setActiveColor(strColor);
}

void PersonalizationGeneralModule::updateVisible(const bool is3DWm)
{
    int row = 0;
    for (auto &&module : m_modules) {
        if ((module->name() == "personalizationTransparencyEffect"
             || module->name() == "personalizationMinimizeEffect"
             || module->name() == "personalizationRoundedEffect")
            && !is3DWm)
            removeChild(module);
        else
            insertChild(row++, module);
    }
}
