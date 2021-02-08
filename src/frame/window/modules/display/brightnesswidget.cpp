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

#include "brightnesswidget.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "window/utils.h"

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
DWIDGET_USE_NAMESPACE;

namespace DCC_NAMESPACE {

namespace display {

const double BrightnessMaxScale = 100.0;
const int PercentageNum = 100;
const double DoubleZero = 0.01; //后端传入的doube指为浮点型，有效位数为2位小数，存在精度丢失

BrightnessWidget::BrightnessWidget(QWidget *parent)
    : QWidget(parent)
    , m_displayModel(nullptr)
    , m_centralLayout(new QVBoxLayout)
    , m_autoLightMode(new SwitchWidget)
    , m_nightShift(new SwitchWidget)
    , m_settingsGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_nightManual(new SwitchWidget)
    , m_cctItem(new TitledSliderItem)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);

    //~ contents_path /display/Brightness
    m_autoLightMode->setTitle(tr("Auto Brightness"));
    m_centralLayout->addWidget(m_autoLightMode);

    m_tempratureColorTitle = new TitleLabel(tr("Color Temperature")); //色温
    m_centralLayout->addWidget(m_tempratureColorTitle);

    //~ contents_path /display/Brightness
    m_nightShift->setTitle(tr("Night Shift"));
    m_nightShift->addBackground();
    m_centralLayout->addWidget(m_nightShift);
    m_nightTips = new DTipLabel(tr("The screen hue will be auto adjusted according to your location"));
    m_nightTips->setForegroundRole(DPalette::TextTips);
    m_nightTips->setWordWrap(true);
    m_nightTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_nightTips->adjustSize();
    m_nightTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_nightTips->setContentsMargins(10, 0, 0, 0);
    m_centralLayout->addWidget(m_nightTips);

    //~ contents_path /display/Brightness
    m_nightManual->setTitle(tr("Change Color Temperature"));
    m_cctItem->setAnnotations({tr("Cool"), "", tr("Warm")});
    m_settingsGroup->appendItem(m_nightManual);
    m_settingsGroup->appendItem(m_cctItem);
    m_centralLayout->addWidget(m_settingsGroup);

    setLayout(m_centralLayout);
}

void BrightnessWidget::setMode(DisplayModel *model)
{
    m_displayModel = model;

    connect(m_autoLightMode, &SwitchWidget::checkedChanged, this, &BrightnessWidget::requestAmbientLightAdjustBrightness);
    connect(m_displayModel, &DisplayModel::adjustCCTmodeChanged, this, &BrightnessWidget::setAdjustCCTmode);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightShift, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_tempratureColorTitle, &TitleLabel::setVisible);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightTips, &QLabel::setVisible);
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
    connect(m_displayModel, &DisplayModel::autoLightAdjustVaildChanged, m_autoLightMode, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::autoLightAdjustSettingChanged, m_autoLightMode, &SwitchWidget::setChecked);

    m_autoLightMode->setVisible(model->autoLightAdjustIsValid());
    m_autoLightMode->setChecked(model->isAudtoLightAdjust());
    int cctmode = model->adjustCCTMode(); //0不调节色温  1  自动调节   2手动调节
    m_nightShift->setVisible(model->redshiftIsValid());
    m_tempratureColorTitle->setVisible(model->redshiftIsValid());
    m_nightManual->setVisible(model->redshiftIsValid());
    m_nightTips->setVisible(model->redshiftIsValid());
    setAdjustCCTmode(cctmode);
    addSlider();
}

void BrightnessWidget::setAdjustCCTmode(int mode)
{
    m_nightShift->blockSignals(true);
    m_nightManual->blockSignals(true);
    m_nightShift->switchButton()->setChecked(mode == 1);
    m_nightManual->switchButton()->setChecked(mode == 2);
    if (m_cctItem) {
        m_cctItem->blockSignals(true);
        m_cctItem->setVisible(m_displayModel->adjustCCTMode() == 2);
        m_cctItem->blockSignals(false);
    }

    m_nightShift->blockSignals(false);
    m_nightManual->blockSignals(false);
}

void BrightnessWidget::showBrightness(Monitor *monitor)
{
    QMap<Monitor *, TitledSliderItem *>::Iterator iter;
    for (iter = m_monitorBrightnessMap.begin(); iter != m_monitorBrightnessMap.end(); ++iter) {
        iter.value()->setVisible(monitor == nullptr || iter.key() == monitor);
    }
}

void BrightnessWidget::addSlider()
{
    auto monList = m_displayModel->monitorList();
    for (auto monitor : monList) {
        if (!monitor->canBrightness()) {
            monList.removeOne(monitor);
        }
    }

    TitleLabel *headTitle = new TitleLabel(tr("Brightness")); //亮度
    m_centralLayout->insertWidget(0, headTitle);
    for (int i = 0; i < monList.size(); ++i) {
        //单独显示每个亮度调节名
        TitledSliderItem *slideritem = new TitledSliderItem(monList[i]->name());

        slideritem->addBackground();
        DCCSlider *slider = slideritem->slider();
        int maxBacklight = static_cast<int>(m_displayModel->maxBacklightBrightness());
        if (maxBacklight == 0) {
            qDebug() << "MinimumBrightness:   " << m_displayModel->minimumBrightnessScale();
            int miniScale = int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
            double brightness = monList[i]->brightness();
            slideritem->setValueLiteral(brightnessToTickInterval(brightness));
            slider->setRange(miniScale, int(BrightnessMaxScale));
            slider->setType(DCCSlider::Vernier);
            slider->setTickPosition(QSlider::TicksBelow);
            slider->setLeftIcon(QIcon::fromTheme("dcc_brightnesslow"));
            slider->setRightIcon(QIcon::fromTheme("dcc_brightnesshigh"));
            slider->setIconSize(QSize(24, 24));
            slider->setTickInterval(int((BrightnessMaxScale - miniScale) / 5.0));
            slider->setValue(int(brightness * BrightnessMaxScale));
            slider->setPageStep(1);

            auto onValueChanged = [=](int pos) {
                this->requestSetMonitorBrightness(monList[i], pos / BrightnessMaxScale);
                this->requestAmbientLightAdjustBrightness(false);
            };

            connect(slider, &DCCSlider::valueChanged, this, onValueChanged);
            connect(slider, &DCCSlider::sliderMoved, this, onValueChanged);

            connect(monList[i], &Monitor::brightnessChanged, this, [=](const double rb) {
                slider->blockSignals(true);
                if ((rb - m_displayModel->minimumBrightnessScale()) < 0.00001) {
                    slideritem->setValueLiteral(QString("%1%").arg(int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale)));
                    slider->setValue(int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale));
                } else {
                    slideritem->setValueLiteral(QString("%1%").arg(int(rb * BrightnessMaxScale)));
                    slider->setValue(int(rb * BrightnessMaxScale));
                }
                slider->blockSignals(false);
            });

            connect(m_displayModel, &DisplayModel::minimumBrightnessScaleChanged,
                    this, [=](const double ms) {
                        double rb = monList[i]->brightness();
                        int tmini = int(ms * PercentageNum);
                        slider->setMinimum(tmini);
                        slider->setTickInterval(int((BrightnessMaxScale - tmini) / 5.0));

                        slider->blockSignals(true);
                        slideritem->setValueLiteral(brightnessToTickInterval(rb));
                        slider->setValue(int(rb * BrightnessMaxScale));
                        slider->blockSignals(false);
                    });
        } else {
            qDebug() << "MinimumBrightness:   " << m_displayModel->minimumBrightnessScale();
            m_miniScales = int(m_displayModel->minimumBrightnessScale() * maxBacklight);
            if (m_miniScales == 0) {
                m_miniScales = 1;
            }
            double brightness = monList[i]->brightness();
            slider->setRange(m_miniScales, maxBacklight);
            slider->setType(DCCSlider::Vernier);
            slider->setTickPosition(QSlider::TicksBelow);
            slider->setLeftIcon(QIcon::fromTheme("dcc_brightnesslow"));
            slider->setRightIcon(QIcon::fromTheme("dcc_brightnesshigh"));
            slider->setIconSize(QSize(24, 24));
            slider->setTickInterval(1);
            slider->setValue(int((brightness + DoubleZero) * maxBacklight));
            slider->setPageStep(1);
            QStringList speedList;
            int j = m_miniScales;
            for (; j <= maxBacklight; j++) {
                speedList << "";
            }
            slideritem->setAnnotations(speedList);

            auto onValueChanged = [=](int pos) {
                this->requestSetMonitorBrightness(monList[i], pos / double(maxBacklight));
                this->requestAmbientLightAdjustBrightness(false);
            };

            connect(slider, &DCCSlider::valueChanged, this, onValueChanged);
            connect(slider, &DCCSlider::sliderMoved, this, onValueChanged);

            connect(monList[i], &Monitor::brightnessChanged, this, [=](const double rb) {
                slider->blockSignals(true);
                if ((rb - m_displayModel->minimumBrightnessScale()) < 0.00001) {
                    slider->setValue(int((m_displayModel->minimumBrightnessScale() + DoubleZero) * maxBacklight));
                } else {
                    slider->setValue(int((rb + DoubleZero) * maxBacklight));
                }
                slider->blockSignals(false);
            });

            connect(m_displayModel, &DisplayModel::minimumBrightnessScaleChanged,
                    this, [=](const double ms) {
                        double rb = monList[i]->brightness();
                        int tmini = int(ms * PercentageNum);
                        slider->setMinimum(tmini);
                        slider->setTickInterval(int((BrightnessMaxScale - tmini) / 5.0));

                        slider->blockSignals(true);
                        slideritem->setValueLiteral(brightnessToTickInterval(rb));
                        slider->setValue(int((rb + DoubleZero) * BrightnessMaxScale));
                        slider->blockSignals(false);
                    });
        }
        m_centralLayout->insertWidget(1, slideritem);
        m_monitorBrightnessMap[monList[i]] = slideritem;
    }

    DCCSlider *cctSlider = m_cctItem->slider();
    cctSlider->setRange(0, 100);
    cctSlider->setType(DCCSlider::Vernier);
    cctSlider->setTickPosition(QSlider::TicksBelow);
    cctSlider->setTickInterval(10);
    cctSlider->setPageStep(1);
    cctSlider->setValue(colorTemperatureToValue(m_displayModel->colorTemperature()));
    if (m_displayModel->redshiftIsValid()) {
        m_cctItem->setVisible(m_displayModel->adjustCCTMode() == 2);
    } else
        m_cctItem->setVisible(false);
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

int BrightnessWidget::colorTemperatureToValue(int kelvin)
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

QString BrightnessWidget::brightnessToTickInterval(const double tb) const
{
    int tmini = int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
    int tnum = int(tb * BrightnessMaxScale);
    tnum = tnum > tmini ? tnum : tmini;
    return QString::number(int(tnum)) + "%";
}

} // namespace display

} // namespace DCC_NAMESPACE
