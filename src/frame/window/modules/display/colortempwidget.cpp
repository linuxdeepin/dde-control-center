// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "colortempwidget.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include "widgets/labels/tipslabel.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include <QLabel>
#include <QVBoxLayout>
#include <DFontSizeManager>

using namespace dcc::widgets;
using namespace dcc::display;
DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace display {

const double BrightnessMaxScale = 100.0;
const int PercentageNum = 100;
const double DoubleZero = 0.01;

ColorTempWidget::ColorTempWidget(QWidget *parent)
    : QWidget(parent)
    , m_displayModel(nullptr)
    , m_centralLayout(new QVBoxLayout(this))
    , m_tempratureColorWidget(new QWidget(this))
    , m_nightShift(new SwitchWidget(this))
    , m_settingsGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_nightManual(new SwitchWidget(this))
    , m_cctItem(new TitledSliderItem(QString(), this))
    , m_nightShiftSpacerItem(new QSpacerItem(0, 10))
    , m_nightTipsSpacerItem(new QSpacerItem(0, 6))
    , m_nightManualSpacerItem(new QSpacerItem(0, 20))
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    m_tempratureColorTitle = new TitleLabel(tr("Color Temperature"), this);

    m_centralLayout->addWidget(m_tempratureColorTitle);

    //~ contents_path /display/Night Shift
    m_nightShift->setTitle(tr("Night Shift"));
    m_nightShift->addBackground();
    m_centralLayout->addSpacerItem(m_nightShiftSpacerItem);
    m_centralLayout->addWidget(m_nightShift);

    m_nightTips = new DTipLabel(tr("The screen hue will be auto adjusted according to your location"), m_tempratureColorWidget);
    m_tempratureColorWidget->setAccessibleName("BrightnessWidget_tempratureColor");
    m_nightTips->setForegroundRole(DPalette::TextTips);
    m_nightTips->setWordWrap(true);
    m_nightTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_nightTips->adjustSize();
    m_nightTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_nightTips->setContentsMargins(10, 0, 0, 0);
    m_centralLayout->addSpacerItem(m_nightTipsSpacerItem);
    m_centralLayout->addWidget(m_nightTips);

    //~ contents_path /display/Change Color Temperature
    m_nightManual->setTitle(tr("Change Color Temperature"));
    m_cctItem->setAnnotations({tr("Cool"), "", tr("Warm")});
    m_settingsGroup->appendItem(m_nightManual);
    m_settingsGroup->appendItem(m_cctItem);
    m_centralLayout->addSpacerItem(m_nightManualSpacerItem);
    m_centralLayout->addWidget(m_settingsGroup);

    m_tempratureColorWidget->setLayout(m_centralLayout);
    GSettingWatcher::instance()->bind("displayColorTemperature", m_tempratureColorWidget);  // 使用GSettings来控制显示状态
    m_centralLayout->addWidget(m_tempratureColorWidget);
    setLayout(m_centralLayout);
}

ColorTempWidget::~ColorTempWidget()
{
    GSettingWatcher::instance()->erase("displayColorTemperature");
}

void ColorTempWidget::setMode(DisplayModel *model)
{
    m_displayModel = model;

    connect(m_displayModel, &DisplayModel::adjustCCTmodeChanged, this, &ColorTempWidget::setAdjustCCTmode);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, this, [=](const bool visible) {
        if ("Hidden" != GSettingWatcher::instance()->getStatus("displayColorTemperature")) {
            setColorTemperatureVisible(visible);
        }
    });
    connect(m_nightManual, &SwitchWidget::checkedChanged, this, [=](const bool enable) {
        if (enable) {
            Q_EMIT requestSetMethodAdjustCCT(2);
        } else {
            Q_EMIT requestSetMethodAdjustCCT(0);
        }
    });
    connect(m_nightShift, &SwitchWidget::checkedChanged, this, [=](const bool enable) {
        if (enable) {
            Q_EMIT requestSetMethodAdjustCCT(1);
        } else {
            Q_EMIT requestSetMethodAdjustCCT(0);
        }
    });

    if ("Hidden" != GSettingWatcher::instance()->getStatus("displayColorTemperature")) {
        setAdjustCCTmode(model->adjustCCTMode()); //0不调节色温  1  自动调节   2手动调节
        setColorTemperatureVisible(model->redshiftIsValid());
    } else {
        setColorTemperatureVisible(false);
    }

    addSlider();
}

void ColorTempWidget::setAdjustCCTmode(int mode)
{
    m_nightShift->blockSignals(true);
    m_nightManual->blockSignals(true);
    m_nightShift->switchButton()->setChecked(mode == 1);
    m_nightManual->switchButton()->setChecked(mode == 2);
    m_cctItem->blockSignals(true);
    //当布局器A中嵌套布局器B时，B中的控件隐藏时，需要先隐藏B再隐藏控件，消除控件闪动问题
    m_settingsGroup->hide();
    m_cctItem->setVisible(m_displayModel->adjustCCTMode() == 2);
    m_settingsGroup->show();
    m_cctItem->blockSignals(false);
    m_nightShift->blockSignals(false);
    m_nightManual->blockSignals(false);
}

void ColorTempWidget::addSlider()
{
    DCCSlider *cctSlider = m_cctItem->slider();
    cctSlider->setRange(0, 100);
    cctSlider->setType(DCCSlider::Vernier);
    cctSlider->setTickPosition(QSlider::TicksBelow);
    cctSlider->setTickInterval(10);
    cctSlider->setPageStep(1);
    cctSlider->setValue(colorTemperatureToValue(m_displayModel->colorTemperature()));
    connect(m_displayModel, &DisplayModel::colorTemperatureChanged, this, [=](int value) {
        cctSlider->blockSignals(true);
        cctSlider->setValue(colorTemperatureToValue(value));
        cctSlider->blockSignals(false);
    });

    connect(cctSlider, &DCCSlider::valueChanged, this, [=](int pos) {
        int kelvin = pos > 50 ? (6500 - (pos - 50) * 100) : (6500 + (50 - pos) * 300);
        this->requestSetColorTemperature(kelvin);
    });
    connect(cctSlider, &DCCSlider::sliderMoved, this, [=](int pos) {
        int kelvin = pos > 50 ? (6500 - (pos - 50) * 100) : (6500 + (50 - pos) * 300);
        this->requestSetColorTemperature(kelvin);
    });
}

int ColorTempWidget::colorTemperatureToValue(int kelvin)
{
    //色温范围有效值10000-25000  值越大，色温越冷，不开启色温时值为6500,超过18000基本看不出变化，小于1500色温实际效果没法看，无实际价值
    //此处取有效至1500-21500
    if (kelvin >= 6500)
        return 50 - (kelvin - 6500) / 300;
    else if (kelvin < 6500 && kelvin >= 1000)
        return 50 - (kelvin - 6500) / 100;
    else
        return 0;
}

void ColorTempWidget::setColorTemperatureVisible(bool visible)
{
    m_nightShiftSpacerItem->changeSize(0, visible ? 10 : 0);
    m_nightTipsSpacerItem->changeSize(0, visible ? 6 : 0);
    m_nightManualSpacerItem->changeSize(0, visible ? 20 : 0);
    m_nightShift->setVisible(visible);
    m_tempratureColorTitle->setVisible(visible);
    m_tempratureColorWidget->setVisible(visible);
    m_nightTips->setVisible(visible);
    m_nightManual->setVisible(visible);
    m_cctItem->setVisible(visible && m_displayModel->adjustCCTMode() == 2);
}

} // namespace display
} // namespace DCC_NAMESPACE

