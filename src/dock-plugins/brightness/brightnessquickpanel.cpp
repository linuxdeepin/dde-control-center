// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnessquickpanel.h"
#include "brightness-constants.h"
#include "brightnessmodel.h"
#include "slidercontainer.h"
#include "brightnesscontroller.h"

#include <DGuiApplicationHelper>

#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>

DGUI_USE_NAMESPACE

#define ICON_SIZE 18
#define BACKSIZE 36

BrightnessQuickPanel::BrightnessQuickPanel(QWidget* parent)
    : QWidget(parent)
    , m_sliderContainer(new SliderContainer(this))
    , m_currentMonitor(nullptr)
{
    initUi();
    initConnection();

    m_sliderContainer->setRange(BrightnessModel::ref().minBrightness(), BrightnessModel::ref().maxBrightness());
    UpdateDisplayStatus();
}

BrightnessQuickPanel::~BrightnessQuickPanel()
{
}

void BrightnessQuickPanel::initUi()
{
    m_sliderContainer->setPageStep(2);
    m_sliderContainer->setIcon(SliderContainer::LeftIcon, QIcon::fromTheme("Brightness-"), 10);
    SliderProxyStyle* proxy = new SliderProxyStyle;
    m_sliderContainer->setSliderProxyStyle(proxy);
    m_sliderContainer->rightIconWidget()->setStyleType(RightIconButton::StyleType::Background);
    m_sliderContainer->setSliderContentsMargin(QMargins(8, 0, 10, 0));

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6, 0, 2, 0);
    mainLayout->addWidget(m_sliderContainer);

    refreshWidget();
}

void BrightnessQuickPanel::initConnection()
{
    connect(&BrightnessModel::ref(), &BrightnessModel::displayModeChanged, this, &BrightnessQuickPanel::UpdateDisplayStatus);
    connect(&BrightnessModel::ref(), &BrightnessModel::minBrightnessChanged, this, [this] {
        m_sliderContainer->setRange(BrightnessModel::ref().minBrightness(), BrightnessModel::ref().maxBrightness());
    });
    connect(&BrightnessModel::ref(), &BrightnessModel::primaryScreenChanged, this, &BrightnessQuickPanel::UpdateDisplayStatus);
    connect(&BrightnessModel::ref(), &BrightnessModel::enabledMonitorListChanged, this, &BrightnessQuickPanel::UpdateDisplayStatus);

    connect(m_sliderContainer, &SliderContainer::sliderValueChanged, this, [this] (int value) {
        if (m_currentMonitor) {
            BrightnessController::ref().setMonitorBrightness(m_currentMonitor, (double)value / BrightnessModel::ref().maxBrightness());
        }
    });
    connect(m_sliderContainer, &SliderContainer::iconClicked, this, [this] (SliderContainer::IconPosition icon) {
        if (icon == SliderContainer::IconPosition::RightIcon) {
            Q_EMIT requestShowApplet();
        }
    });
    connect(m_sliderContainer, &SliderContainer::panelClicked, this, &BrightnessQuickPanel::requestShowApplet);
    refreshWidget();
}

void BrightnessQuickPanel::UpdateDisplayStatus()
{
    const auto &monitors = BrightnessModel::ref().enabledMonitors();
    Monitor *mo = monitors.size() > 0 ? monitors.first() : nullptr;
    if (mo == m_currentMonitor || !mo)
        return;

    if (m_currentMonitor)
        m_currentMonitor->disconnect(this);

    m_currentMonitor = mo;
    connect(m_currentMonitor, &Monitor::brightnessChanged, this, [this] (const double value) {
        m_sliderContainer->blockSignals(true);
        if ((value - BrightnessModel::ref().minimumBrightnessScale()) < 0.00001) {
            m_sliderContainer->updateSliderValue(BrightnessModel::ref().minBrightness());
        } else {
            m_sliderContainer->updateSliderValue(int(value * BrightnessModel::ref().maxBrightness()));
        }
        m_sliderContainer->blockSignals(false);
    });
    refreshWidget();
}

/**
 * @brief 刷新左右图标、滑动条位置
 *
 */
void BrightnessQuickPanel::refreshWidget()
{
    if (m_currentMonitor) {
        const QString &iconName = m_currentMonitor->name().contains("eDP", Qt::CaseInsensitive) ? QStringLiteral("laptop") : QStringLiteral("external-display");
        m_sliderContainer->setIcon(SliderContainer::RightIcon, QIcon::fromTheme(iconName), 10);
        m_sliderContainer->updateSliderValue(m_currentMonitor->brightness() * 100);
    }
}
