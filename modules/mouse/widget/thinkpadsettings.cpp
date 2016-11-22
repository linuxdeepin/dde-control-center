#include "thinkpadsettings.h"

ThinkpadSettings::ThinkpadSettings(QObject *parent)
{
    Q_UNUSED(parent);
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    speedSlider = new SpeedSlider;
    m_title     = new ModuleTitle(tr("Thinkpad Track Point"));
    speedSlider->setTitle(tr("Pointer Speed"));
    speedSlider->setMaxValue(TrackMoveSpeedMax);
    speedSlider->setMinValue(TrackMoveSpeedMin);
    speedSlider->setStep(TrackMoveSpeedStep);

    m_mainGroup->appendItem(speedSlider);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(speedSlider, &SpeedSlider::requestSetSliderValue, this, &ThinkpadSettings::requestSetSliderValue);
    setObjectName("ThinkpadSettings");
}

void ThinkpadSettings::setModel(MouseModelThinkpadSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelMouseSettings::sliderValueChanged, this, &ThinkpadSettings::setSliderValue);
    setSliderValue(m_baseSettings->getSliderValue());
}

void ThinkpadSettings::setSliderValue(const int &value)
{
    speedSlider->setValue(value);
}
