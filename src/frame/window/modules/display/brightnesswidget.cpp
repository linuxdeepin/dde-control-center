// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnesswidget.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include "widgets/settingsheaderitem.h"
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
const double DoubleZero = 0.01; //后端传入的doube指为浮点型，有效位数为2位小数，存在精度丢失

BrightnessWidget::BrightnessWidget(QWidget *parent)
    : QWidget(parent)
    , m_displayModel(nullptr)
    , m_centralLayout(new QVBoxLayout(this))
    , m_autoLightSpacerItem(new QSpacerItem(0, 10))
    , m_autoLightMode(new SwitchWidget(this))
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    //~ contents_path /display/Brightness
    m_brightnessTitle = new TitleLabel(tr("Brightness"), this);
    //~ contents_path /display/Color Temperature
    GSettingWatcher::instance()->bind("displayLightLighting", m_brightnessTitle);  // 使用GSettings来控制显示状态
    m_centralLayout->addWidget(m_brightnessTitle);

    m_centralLayout->addSpacerItem(m_autoLightSpacerItem);

    //~ contents_path /display/Auto Brightness
    m_autoLightMode->setTitle(tr("Auto Brightness"));
    m_autoLightMode->addBackground();
    m_autoLightMode->setVisible(false);
    m_centralLayout->addWidget(m_autoLightMode);

    setLayout(m_centralLayout);
}

BrightnessWidget::~BrightnessWidget() = default;

void BrightnessWidget::setMode(DisplayModel *model)
{
    m_displayModel = model;

    connect(m_autoLightMode, &SwitchWidget::checkedChanged, this, &BrightnessWidget::requestAmbientLightAdjustBrightness);
    connect(m_displayModel, &DisplayModel::autoLightAdjustVaildChanged, this, [=](const bool enable) {
        m_autoLightSpacerItem->changeSize(0, enable ? 10 : 0);
        m_autoLightMode->setVisible(enable);
    });
    connect(m_displayModel, &DisplayModel::autoLightAdjustSettingChanged, m_autoLightMode, &SwitchWidget::setChecked);

    addSlider();
}

void BrightnessWidget::showBrightness(Monitor *monitor)
{
    bool bTitle(false);
    QMap<Monitor *, QWidget *>::const_iterator iter;
    for (iter = m_monitorBrightnessMap.cbegin(); iter != m_monitorBrightnessMap.cend(); ++iter) {
        iter.value()->setVisible(monitor == nullptr || iter.key() == monitor);
        if (!bTitle && (monitor == nullptr || iter.key() == monitor)) {
            bTitle = true;
        }
    }
    m_brightnessTitle->setVisible(bTitle);
}

void BrightnessWidget::addSlider()
{
    auto monList = m_displayModel->monitorList();
    for (auto monitor : monList) {
        if (!monitor->canBrightness()) {
            monList.removeOne(monitor);
        }
    }

    for (int i = 0; i < monList.size(); ++i) {
        //单独显示每个亮度调节名
        TitledSliderItem *slideritem = new TitledSliderItem(monList[i]->name(), this);
        slideritem->setAccessibleName("BrightnessWidget_TitledSliderItem");
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
                this->m_autoLightMode->setChecked(false);
            };

            connect(slider, &DCCSlider::valueChanged, this, onValueChanged);
            connect(slider, &DCCSlider::sliderMoved, this, onValueChanged);

            connect(monList[i], &Monitor::brightnessChanged, this, [=](const double rb) {
                slider->blockSignals(true);
                int iValue = 0;
                if ((rb - m_displayModel->minimumBrightnessScale()) < 0.00001) {
                    iValue = qRound(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
                } else {
                    iValue = qRound(rb * BrightnessMaxScale);
                }
                slideritem->setValueLiteral(QString("%1%").arg(iValue));
                if (!slideritem->isSliderPressed())
                    slider->setValue(iValue);
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
            slideritem->setValueLiteral(brightnessToTickInterval(brightness));
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
                this->m_autoLightMode->setChecked(false);
            };

            connect(slider, &DCCSlider::valueChanged, this, onValueChanged);
            connect(slider, &DCCSlider::sliderMoved, this, onValueChanged);

            connect(monList[i], &Monitor::brightnessChanged, this, [=](const double rb) {
                slider->blockSignals(true);
                slideritem->setValueLiteral(brightnessToTickInterval(rb));
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
        QWidget *brightnessSlideritem = new QWidget(this);
        brightnessSlideritem->setAccessibleName("brightnessSlideritem");
        QVBoxLayout *sliderLayout = new QVBoxLayout(brightnessSlideritem);
        sliderLayout->setContentsMargins(0, 10, 0, 0);
        GSettingWatcher::instance()->bind("displayLightLighting", slideritem);  // 使用GSettings来控制显示状态
        sliderLayout->addWidget(slideritem);
        brightnessSlideritem->setLayout(sliderLayout);
        m_centralLayout->insertWidget(1, brightnessSlideritem);
        m_monitorBrightnessMap[monList[i]] = brightnessSlideritem;
    }
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
