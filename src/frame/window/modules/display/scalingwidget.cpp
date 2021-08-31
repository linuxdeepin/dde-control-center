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
#include "window/gsettingwatcher.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "widgets/dccslider.h"

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
    , m_centralLayout(new QVBoxLayout(this))
    , m_tipWidget(new QWidget(this))
    , m_tipLabel(new DTipLabel(tr("The monitor only supports 100% display scaling"), this))
    , m_slider(new TitledSliderItem(QString(), this))
{
    //初始化列表无法进行静态翻译
    //~ contents_path /display
    m_title = new TitleLabel(tr("Display Scaling"), this);
    m_title->setText(tr("Display Scaling"));

    m_tipWidget->setAccessibleName("ScalingWidget_tipWidget");
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(8);
    GSettingWatcher::instance()->bind("displayScaling", m_title);  // 使用GSettings来控制显示状态
    m_centralLayout->addWidget(m_title);

    m_tipLabel->setForegroundRole(DPalette::TextTips);
    m_tipLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QVBoxLayout *tipLayout = new QVBoxLayout(m_tipWidget);
    tipLayout->setContentsMargins(10, 0, 0, 0);
    GSettingWatcher::instance()->bind("displayScaling", m_tipLabel);  // 使用GSettings来控制显示状态
    tipLayout->addWidget(m_tipLabel);
    m_tipWidget->setLayout(tipLayout);
    m_centralLayout->addWidget(m_tipWidget);

    m_slider->addBackground();
    GSettingWatcher::instance()->bind("displayScaling", m_slider);  // 使用GSettings来控制显示状态
    m_centralLayout->addWidget(m_slider);
    setLayout(m_centralLayout);
}

ScalingWidget::~ScalingWidget()
{
    GSettingWatcher::instance()->erase("displayScaling");
}

void ScalingWidget::setModel(DisplayModel *model)
{
    m_displayModel = model;

    addSlider();
}

void ScalingWidget::addSlider()
{
    if (m_displayModel->monitorList().size() == 0)
        return;

    onResolutionChanged();
    DCCSlider *slider = m_slider->slider();
    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        if (m_displayModel->uiScale() != m_scaleList[value - 1].toFloat()) {
            Q_EMIT requestUiScaleChange(m_scaleList[value - 1].toFloat());
        }
    });
    connect(m_displayModel, &DisplayModel::uiScaleChanged, this, [=](const double scale) {
        slider->blockSignals(true);
        qDebug() << "monitor scaleCahnged ,scale :" << convertToSlider(scale);
        slider->setValue(convertToSlider(scale));
        slider->blockSignals(false);
    });

    for (auto moni : m_displayModel->monitorList()) {
        connect(moni, &Monitor::currentModeChanged, this, &ScalingWidget::onResolutionChanged);
        connect(moni, &Monitor::enableChanged, this, &ScalingWidget::onResolutionChanged);
    }
}

void ScalingWidget::onResolutionChanged()
{
    QStringList fscaleList = {"1.0", "1.25", "1.5", "1.75", "2.0", "2.25", "2.5", "2.75", "3.0"};
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

    //如果仅一个缩放值可用
    if (fscaleList.size() <= 1) {
        fscaleList.clear();
        fscaleList.append(QStringList() << "1.0" << "1.0");
        m_tipWidget->setVisible(true);
    } else {
        m_tipWidget->setVisible(false);
    }

    m_scaleList = fscaleList;
    m_slider->setAnnotations(m_scaleList);
    DCCSlider *slider = m_slider->slider();
    slider->blockSignals(true);
    slider->setRange(1, m_scaleList.size());
    slider->setPageStep(1);
    slider->setValue(convertToSlider(m_displayModel->uiScale() > 1.0 ? m_displayModel->uiScale() : 1.0));
    slider->update();
    slider->blockSignals(false);
};

QStringList ScalingWidget::getScaleList(const Resolution &r)
{
    const QStringList tvstring = {"1.0", "1.25", "1.5", "1.75", "2.0", "2.25", "2.5", "2.75", "3.0"};
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

} // namespace display
} // namespace DCC_NAMESPACE
