// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnessapplet.h"
#include "brightnessmodel.h"
#include "monitor.h"
#include "brightnesscontroller.h"
#include "constants.h"

#include <QScroller>
#include <QVBoxLayout>
#include <DBlurEffectWidget>
#include <DFontSizeManager>

static const int SLIDER_ITEM_HEIGHT = 56;
static const int ITEM_SPACING = 10;
#define MONITOR_NAME_PROPERTY "MonitorName"

DWIDGET_USE_NAMESPACE

BrightnessApplet::BrightnessApplet(QWidget* parent)
    : QWidget(parent)
    , m_titleWidget(new QWidget(this))
    , m_scrollArea(new QScrollArea(this))
    , m_monitorsLayout(nullptr)
    , m_jumpSettingButton(new JumpSettingButton(this))
    , m_minHeight(-1)
{
    initUI();
    initConnections();

    updateMonitors();
}

BrightnessApplet::~BrightnessApplet()
{

}

void BrightnessApplet::initUI()
{
    setFixedWidth(Dock::DOCK_POPUP_WIDGET_WIDTH);

    auto title = new QLabel(tr("Brightness"), m_titleWidget);
    DFontSizeManager::instance()->bind(title, DFontSizeManager::T9, QFont::Medium);
    auto titleLayout = new QHBoxLayout(m_titleWidget);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addSpacing(10);
    titleLayout->addWidget(title);
    titleLayout->addStretch();

    m_monitorsLayout = new QVBoxLayout;
    m_monitorsLayout->setContentsMargins(0, 0, 0, 0);
    m_monitorsLayout->setSpacing(ITEM_SPACING);
    auto *widget = new QWidget;
    widget->setAttribute(Qt::WA_TranslucentBackground);
    widget->setLayout(m_monitorsLayout);
    m_scrollArea->setWidget(widget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);

    QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(this->window());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_jumpSettingButton->setIcon(QIcon::fromTheme("open-arrow"));
    m_jumpSettingButton->setDescription(tr("Display settings"));
    m_jumpSettingButton->setDccPage("display", "");

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(m_titleWidget);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(m_scrollArea);
    mainLayout->addStretch(0);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_jumpSettingButton);
}

void BrightnessApplet::initConnections()
{
    connect(&BrightnessModel::ref(), &BrightnessModel::enabledMonitorListChanged, this, &BrightnessApplet::updateMonitors);
    connect(&BrightnessModel::ref(), &BrightnessModel::displayModeChanged, this, &BrightnessApplet::updateMonitors);
    connect(&BrightnessModel::ref(), &BrightnessModel::primaryScreenChanged, this, &BrightnessApplet::updateMonitors);
    connect(m_jumpSettingButton, &JumpSettingButton::showPageRequestWasSended, this, &BrightnessApplet::requestHideApplet);
}

void BrightnessApplet::resizeScrollArea()
{
    int itemSize = 0;
    for (int i = 0; i < m_monitorsLayout->count(); i++) {
        if (qobject_cast<SliderContainer*>(m_monitorsLayout->itemAt(i)->widget()))
            itemSize++;
    }
    const int scrollAreaHeight = itemSize * SLIDER_ITEM_HEIGHT + (itemSize - 1) * ITEM_SPACING;
    m_scrollArea->setFixedHeight(qMax(0, scrollAreaHeight));
    m_scrollArea->updateGeometry();
    resize(width(), qMax(m_minHeight, scrollAreaHeight + 10 * 2 + m_jumpSettingButton->height()));
}

void BrightnessApplet::addMonitor(Monitor *monitor)
{
    auto container = new SliderContainer(this);
    container->setFixedHeight(SLIDER_ITEM_HEIGHT);
    container->addBackground();
    auto dSlider = new Dtk::Widget::DSlider();
    dSlider->setMouseWheelEnabled(true);
    container->setSlider(dSlider);
    container->setRange(BrightnessModel::ref().minBrightness(), BrightnessModel::ref().maxBrightness());
    container->setTip(monitor->name(), SliderContainer::LeftTip);
    container->setTip(QString::number(monitor->brightness() * 100) + "%", SliderContainer::RightTip);
    container->setIcon(SliderContainer::LeftIcon, QIcon::fromTheme("Brightness-"), 0);
    container->setIcon(SliderContainer::RightIcon, QIcon::fromTheme("Brightness+"), 0);
    container->setButtonsSize(QSize(16, 16));
    container->updateSliderValue(monitor->brightness() * 100);
    container->setProperty(MONITOR_NAME_PROPERTY, monitor->name());
    container->slider()->setEnabled(monitor->canBrightness());
    m_monitorsLayout->addWidget(container);

    connect(&BrightnessModel::ref(), &BrightnessModel::minBrightnessChanged, container, [container] {
        container->setRange(BrightnessModel::ref().minBrightness(), BrightnessModel::ref().maxBrightness());
    });
    connect(container, &SliderContainer::sliderValueChanged, this, [container] (int value) {
        const auto &monitor = BrightnessModel::ref().monitor(container->property(MONITOR_NAME_PROPERTY).toString());
        if (monitor) {
            BrightnessController::ref().setMonitorBrightness(monitor, (double)value / BrightnessModel::ref().maxBrightness());
        }
    });
    connect(monitor, &Monitor::brightnessChanged, container, [container] (double value) {
        container->blockSignals(true);
        if ((value - BrightnessModel::ref().minimumBrightnessScale()) < 0.00001) {
            container->updateSliderValue(BrightnessModel::ref().minBrightness());
        } else {
            container->updateSliderValue(int(value * BrightnessModel::ref().maxBrightness()));
        }
        container->setTip(QString::number(value * 100) + "%", SliderContainer::RightTip);
        container->blockSignals(false);
    });
}

void BrightnessApplet::updateMonitors()
{
    const auto &monitors =BrightnessModel::ref().enabledMonitors();
    // 获取现在界面上显示的屏幕
    QMap<QString, SliderContainer*> sliderContainers;
    for (int i = 0; i < m_monitorsLayout->count(); i++) {
        auto slider = qobject_cast<SliderContainer*>(m_monitorsLayout->itemAt(i)->widget());
        if (slider)
            sliderContainers.insert(slider->property(MONITOR_NAME_PROPERTY).toString(), slider);
    }

    // 移除所有屏幕，按照排序依次添加
    for (const auto &slider : sliderContainers.values()) {
        m_monitorsLayout->removeWidget(slider);
    }

    for (const auto &monitor : monitors) {
        if (!sliderContainers.contains(monitor->name())) {
            addMonitor(monitor);
        } else {
            m_monitorsLayout->addWidget(sliderContainers.value(monitor->name()));
            sliderContainers.remove(monitor->name());
        }
    }

    // 剩下的就是不用展示的屏幕
    for (const auto &slider : sliderContainers.values()) {
        m_monitorsLayout->removeWidget(slider);
        slider->deleteLater();
    }

    resizeScrollArea();
}

void BrightnessApplet::setAppletMinHeight(int minHeight)
{
    m_minHeight = minHeight;
    resizeScrollArea();
}

void BrightnessApplet::resizeEvent(QResizeEvent *event)
{
    resizeScrollArea();

    QWidget::resizeEvent(event);
}

void BrightnessApplet::onContainerChanged(int container)
{
    m_titleWidget->setVisible(Dock::APPLET_CONTAINER_QUICK_PANEL != container);
}
