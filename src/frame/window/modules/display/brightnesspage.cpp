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

#include "brightnesspage.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/brightnessitem.h"
#include "window/utils.h"

#include "widgets/labels/tipslabel.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QList>
#include <DFontSizeManager>

using namespace dcc::widgets;
using namespace dcc::display;
DWIDGET_USE_NAMESPACE;

namespace DCC_NAMESPACE {

namespace display {

const double BrightnessMaxScale = 100.0;
const int PercentageNum = 100;

BrightnessPage::BrightnessPage(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->setContentsMargins(ThirdPageContentsMargins);

    m_nightShift = new SwitchWidget;
    //~ contents_path /display/Brightness
    m_nightShift->setTitle(tr("Night Shift"));
    m_centralLayout->addWidget(m_nightShift);

///    //~ contents_path /display/Brightness
    m_nightTips = new DTipLabel(tr("The screen hue will be auto adjusted according to your location"));
    m_nightTips->setWordWrap(true);
    m_nightTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_nightTips->adjustSize();
    m_nightTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_nightTips->setContentsMargins(10, 0, 0, 0);
    m_centralLayout->addWidget(m_nightTips);

    m_autoLightMode = new SwitchWidget;
    //~ contents_path /display/Brightness
    m_autoLightMode->setTitle(tr("Auto Brightness"));
    m_centralLayout->addWidget(m_autoLightMode);

    setLayout(m_centralLayout);
}

void BrightnessPage::setMode(DisplayModel *model)
{
    m_displayModel = model;

    connect(m_autoLightMode, &SwitchWidget::checkedChanged, this, &BrightnessPage::requestAmbientLightAdjustBrightness);
    connect(m_displayModel, &DisplayModel::nightModeChanged, m_nightShift, &SwitchWidget::setChecked);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightShift, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightTips, &QLabel::setVisible);

    connect(m_nightShift, &SwitchWidget::checkedChanged, this, &BrightnessPage::requestSetNightMode);
    connect(m_displayModel, &DisplayModel::redshiftSettingChanged, m_nightShift->switchButton(), &DSwitchButton::setDisabled);
    connect(m_displayModel, &DisplayModel::autoLightAdjustVaildChanged, m_autoLightMode, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::autoLightAdjustSettingChanged, m_autoLightMode, &SwitchWidget::setChecked);

    m_autoLightMode->setVisible(model->autoLightAdjustIsValid());
    m_autoLightMode->setChecked(model->isAudtoLightAdjust());

    m_nightShift->setVisible(model->redshiftIsValid());
    m_nightTips->setVisible(model->redshiftIsValid());
    m_nightShift->setChecked(model->isNightMode());
    m_nightShift->setDisabled(model->redshiftSetting());

    addSlider();
    m_centralLayout->addStretch(1);
}

void BrightnessPage::addSlider()
{
    auto monList = m_displayModel->monitorList();
    if (m_displayModel->getSystemType()) {
        monList.clear();
        for (auto monitor : m_displayModel->monitorList()) {
            QString monitorName = monitor->name();
            if(monitorName.contains("eDP")) {
                monList.append(monitor);
            }
        }
    }

    for (int i = 0; i < monList.size(); ++i) {
        //单独显示每个亮度调节名
        TitledSliderItem *slideritem = new TitledSliderItem(monList[i]->name());
        slideritem->addBackground();

        qDebug() << "MinimumBrightness:   " << m_displayModel->minimumBrightnessScale();
        int miniScale = int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
        double brightness = monList[i]->brightness();
        slideritem->setValueLiteral(brightnessToTickInterval(brightness));

        DCCSlider *slider = slideritem->slider();
        slider->setRange(miniScale, int(BrightnessMaxScale));
        slider->setType(DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setLeftIcon(QIcon::fromTheme("dcc_brightnesslow"));
        slider->setRightIcon(QIcon::fromTheme("dcc_brightnesshigh"));
        slider->setIconSize(QSize(24, 24));
        slider->setTickInterval(int((BrightnessMaxScale - miniScale) / 5.0));
        slider->setValue(int(brightness * BrightnessMaxScale));
        slider->setPageStep(1);


        auto onValueChanged = [ = ](int pos) {
            this->requestSetMonitorBrightness(monList[i], pos / BrightnessMaxScale);
            this->requestAmbientLightAdjustBrightness(false);
        };

        connect(slider, &DCCSlider::valueChanged, this, onValueChanged);
        connect(slider, &DCCSlider::sliderMoved, this, onValueChanged);

        connect(monList[i], &Monitor::brightnessChanged, this, [ = ](const double rb) {
            qDebug() << "received Monitor::brightnessChanged" << rb << sender()->objectName();
            if (slideritem->isSliderPressed()) {
                return;
            }
            slider->blockSignals(true);
            int iValue = 0;
            if ((rb - m_displayModel->minimumBrightnessScale()) < 0.00001) {
                iValue = qRound(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
                slideritem->setValueLiteral(QString("%1%").arg(iValue));
                slider->setValue(iValue);
            } else {
                iValue = qRound(rb * BrightnessMaxScale);
                slideritem->setValueLiteral(QString("%1%").arg(iValue));
                slider->setValue(iValue);
            }
            slider->blockSignals(false);
        });

        connect(m_displayModel, &DisplayModel::minimumBrightnessScaleChanged,
        this, [ = ](const double ms) {
            double rb = monList[i]->brightness();
            int tmini = int(ms * PercentageNum);
            slider->setMinimum(tmini);
            slider->setTickInterval(int((BrightnessMaxScale - tmini) / 5.0));
            qDebug() << "received DisplayModel::minimumBrightnessScaleChanged" << rb << tmini;

            slider->blockSignals(true);
            slideritem->setValueLiteral(brightnessToTickInterval(rb));
            slider->setValue(int(rb * BrightnessMaxScale));
            slider->blockSignals(false);
        });

        m_centralLayout->addWidget(slideritem);
    }
}

QString BrightnessPage::brightnessToTickInterval(const double tb) const
{
    int tmini = int(m_displayModel->minimumBrightnessScale() * BrightnessMaxScale);
    int tnum = int(tb * BrightnessMaxScale);
    tnum = tnum > tmini ? tnum : tmini;
    return QString::number(int(tnum)) + "%";
}

}

}
