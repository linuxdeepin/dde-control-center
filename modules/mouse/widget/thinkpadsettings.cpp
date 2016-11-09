#include "thinkpadsettings.h"

ThinkpadSettings::ThinkpadSettings(QObject *parent)
{
    Q_UNUSED(parent);
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    speedSlider = new SpeedSlider;
    speedSlider->setTitle(tr("Pointer Speed"));
    m_mainGroup->appendItem(speedSlider);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(speedSlider, &SpeedSlider::requestSetSliderValue, this, &ThinkpadSettings::requestSetSliderValue);
}

void ThinkpadSettings::setModel(MouseModelThinkpadSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelMouseSettings::sliderValueChanged, this, &ThinkpadSettings::setSliderValue);
    setSliderValue(m_baseSettings->getSliderValue());
    setSliderMaxValue(m_baseSettings->getSliderMaxValue());
    setSliderMinValue(m_baseSettings->getSliderMinValue());
    setSliderStep(m_baseSettings->getSliderStep());
}

void ThinkpadSettings::setSliderValue(const int &value)
{
    speedSlider->setValue(value);
}

void ThinkpadSettings::setSliderMaxValue(const int &value)
{
    speedSlider->setMaxValue(value);
}

void ThinkpadSettings::setSliderMinValue(const int &value)
{
    speedSlider->setMinValue(value);
}

void ThinkpadSettings::setSliderStep(const int &value)
{
    speedSlider->setStep(value);
}
