#include "brightnessitem.h"
#include "dccslider.h"

#include <QHBoxLayout>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace display {

#define BRIGHTNESS_MUL  1000

BrightnessItem::BrightnessItem(QWidget *parent)
    : SettingsItem(parent),

      m_leftIcon(new DImageButton),
      m_rightIcon(new DImageButton),
      m_brightnessSlider(new DCCSlider(DCCSlider::Normal))
{
    m_leftIcon->setNormalPic(":/display/themes/dark/icons/light_low.png");
    m_rightIcon->setNormalPic(":/display/themes/dark/icons/light_high.png");
    m_brightnessSlider->setOrientation(Qt::Horizontal);
    m_brightnessSlider->setMaximum(BRIGHTNESS_MUL);
    m_brightnessSlider->setMinimum(0.2 * BRIGHTNESS_MUL);
    m_brightnessSlider->setTracking(true);

    QHBoxLayout *centralLayout = new QHBoxLayout;

    centralLayout->addWidget(m_leftIcon);
    centralLayout->addWidget(m_brightnessSlider);
    centralLayout->addWidget(m_rightIcon);
    centralLayout->setContentsMargins(10, 5, 10, 5);
    centralLayout->setSpacing(5);

    setLayout(centralLayout);

    connect(m_brightnessSlider, &QSlider::valueChanged, [this](const double value) { emit requestSetMonitorBrightness(m_monitor, double(value) / BRIGHTNESS_MUL); });
}

void BrightnessItem::setValue(const double value)
{
    m_brightnessSlider->blockSignals(true);
    m_brightnessSlider->setValue(value * BRIGHTNESS_MUL);
    m_brightnessSlider->blockSignals(false);
}

void BrightnessItem::setMonitor(Monitor * const mon)
{
    m_monitor = mon;
}

}

}

