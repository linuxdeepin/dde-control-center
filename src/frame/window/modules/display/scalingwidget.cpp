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

#include "scalingwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"

#include "widgets/dccslider.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"

#include <QVBoxLayout>

const float MinScreenWidth = 1024.0f;
const float MinScreenHeight = 768.0f;

using namespace dcc::display;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

namespace display {

ScalingWidget::ScalingWidget(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout)
{
    m_centralLayout->setMargin(0);

    m_tip = new TitleLabel(tr("Display Scaling"));
    GSettingWatcher::instance()->bind("displayScaling", m_tip);  // 使用GSettings来控制显示状态
    DFontSizeManager::instance()->bind(m_tip, DFontSizeManager::T5, QFont::DemiBold);

    m_centralLayout->addWidget(m_tip);
    setLayout(m_centralLayout);
}

ScalingWidget::~ScalingWidget()
{
    GSettingWatcher::instance()->erase("displayScaling");
}

void ScalingWidget::setModel(DisplayModel *model)
{
    m_displayModel = model;

    setupSliders();
#ifdef ALL_MONITOR_SAME_SCALE
#else
    connect(m_displayModel, &DisplayModel::uiScaleChanged, this, [=](const double scale) {
        for (auto widget : m_sliders) {
            DCCSlider *slider = widget->slider();
            slider->blockSignals(true);
            slider->setValue(convertToSlider(scale));
            slider->blockSignals(false);

            widget->setValueLiteral(QString::number(scale));
        }
    });
#endif
}

void ScalingWidget::setupSliders()
{
#ifdef ALL_MONITOR_SAME_SCALE
    if (m_slider)
        m_slider->deleteLater();
    addSlider(0);
#else
    for (auto &s : m_sliders)
        delete s;
    m_sliders.clear();
    for (int i = 0; i < m_displayModel->monitorList().size(); ++i)
        addSlider(i);
#endif
}

#ifdef ALL_MONITOR_SAME_SCALE
void ScalingWidget::addSlider(int monitorID)
{
    Q_UNUSED(monitorID)

    if (m_displayModel->monitorList().size() == 0)
        return;

    m_slider = new TitledSliderItem("");
    m_slider->addBackground();
    const QStringList maxList = {"1.0", "1.25", "1.5", "1.75", "2.0", "2.25", "2.5", "2.75", "3.0"};
    ;
    QStringList fscaleList = maxList;
    for (auto moni : m_displayModel->monitorList()) {
        if (!moni->enable()) {
            continue;
        }
        auto tmode = moni->currentMode();
        // 后端传入currentMode值可能为0
        if (tmode.width() == 0 || tmode.height() == 0) {
            return;
        }
        auto ts = getScaleList(tmode);
        fscaleList = ts.size() < fscaleList.size() ? ts : fscaleList;
    }

    //如果仅一个缩放值可用，则不显示
    auto scale = m_displayModel->uiScale();
    if (fscaleList.size() <= 1 && fabs(scale - 1.0) < 0.000001) {
        return;
    }

    //如果当前缩放大于可用缩放，则显示至当前缩放
    if (fscaleList.size() == 0) {
        fscaleList.append("1.0");
    }

    DCCSlider *slider = m_slider->slider();
    slider->setRange(1, fscaleList.size());
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    m_slider->setAnnotations(fscaleList);
    m_centralLayout->addWidget(m_slider);
    GSettingWatcher::instance()->bind("displayScaling", m_slider);  // 使用GSettings来控制显示状态

    double scaling = m_displayModel->uiScale();
    if (scaling < 1.0)
        scaling = 1.0;
    slider->setValue(convertToSlider(scaling));

    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        Q_EMIT requestUiScaleChange(convertToScale(value));
    });
    connect(m_displayModel, &DisplayModel::uiScaleChanged, this, [=](const double scale) {
        slider->blockSignals(true);
        qDebug() << "monitor scaleCahnged ,scale :" << convertToSlider(scale);
        slider->setValue(convertToSlider(scale));
        slider->blockSignals(false);
    });

    for (auto moni : m_displayModel->monitorList()) {
        connect(moni, &Monitor::currentModeChanged, this, &ScalingWidget::onResolutionChanged);
    }
}
#else
void ScalingWidget::addSlider(int monitorID)
{
    qDebug() << "set scaling" << ~monitorID;
    auto moni = m_displayModel->monitorList()[monitorID];
    m_sliders.push_back(new TitledSliderItem(moni->name()));

    auto fscaleList = getScaleList(moni->currentMode());
    TitledSliderItem *slideritem = m_sliders.back();
    slideritem->addBackground();

    DCCSlider *slider = slideritem->slider();
    slider->setRange(1, fscaleList.size());
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    slideritem->setAnnotations(fscaleList);
    m_centralLayout->addWidget(slideritem);

    double scaling = m_displayModel->monitorScale(moni);
    if (scaling < 1.0)
        scaling = 1.0;
    slider->setSliderPosition(convertToSlider(scaling));

    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        Q_EMIT requestIndividualScaling(moni, convertToScale(value));
    });

    slider->setValue(convertToSlider(scaling));

    connect(moni, &Monitor::scaleChanged, this, [=](const double scale) {
        slider->blockSignals(true);
        qDebug() << "monitor scaleCahnged ,scale :" << convertToSlider(scale);
        slider->setSliderPosition(convertToSlider(scale));
        slider->blockSignals(false);
    });

    connect(moni, &Monitor::currentModeChanged, this, [=](const Resolution &r) {
        auto fscaleList = getScaleList(r);
        if (fscaleList.size() < slider->value()) {
            qDebug() << "分辨率被设置，当前缩放会使屏幕过小，大部分窗口将无法被完整显示！";
        } else {
            auto tv = slider->value();
            slider->blockSignals(true);
            slider->setRange(1, fscaleList.size());
            slider->setAnnotations(fscaleList);
            slider->setValue(tv);
            slider->blockSignals(false);
        }
    });
}
#endif

void ScalingWidget::onResolutionChanged()
{
    auto pmoni = m_displayModel->monitorList()[0];
    QStringList fscaleList = getScaleList(pmoni->currentMode());
    for (auto moni : m_displayModel->monitorList()) {
        auto ts = getScaleList(moni->currentMode());
        fscaleList = ts.size() < fscaleList.size() ? ts : fscaleList;
    }

    auto tslider = m_slider->slider();
    if (fscaleList.size() < tslider->value()) {
        qDebug() << "分辨率被设置，当前缩放会使屏幕过小，大部分窗口将无法被完整显示！";
    } else {
        auto tv = tslider->value();
        tslider->blockSignals(true);
        tslider->setRange(1, fscaleList.size());
        tslider->setAnnotations(fscaleList);
        tslider->setValue(tv);
        tslider->blockSignals(false);
    }
};

QStringList ScalingWidget::getScaleList(const Resolution &r)
{
    QStringList tvstring;
    tvstring << "1.0"
             << "1.25"
             << "1.5"
             << "1.75"
             << "2.0"
             << "2.25"
             << "2.5"
             << "2.75"
             << "3.0";

    QStringList fscaleList;
    auto maxWScale = r.width() / MinScreenWidth;
    auto maxHScale = r.height() / MinScreenHeight;
    auto maxScale = maxWScale < maxHScale ? maxWScale : maxHScale;
    maxScale = maxScale < 3.0f ? maxScale : 3.0f;
    for (int idx = 0; idx * 0.25f + 1.0f <= maxScale; ++idx) {
        fscaleList << tvstring[idx];
    }

    return fscaleList;
}

int ScalingWidget::convertToSlider(const double value)
{
    //remove base scale (100), then convert to 1-based value
    //with a stepping of 25
    return int(round(double(value * 100 - 100) / 25)) + 1;
}

double ScalingWidget::convertToScale(const int value)
{
    return 1.0 + (value - 1) * 0.25;
}

} // namespace display
} // namespace DCC_NAMESPACE
