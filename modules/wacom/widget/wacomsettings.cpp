#include "wacomsettings.h"

WacomSettings::WacomSettings(QObject *parent)
{
    Q_UNUSED(parent);
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    m_pressureSlider = new WacomPressure;
    m_pressureSlider->setTitle(tr("Pressure Sensitive"));
    m_pressureSlider->setMaxValue(PressureMaxValue);
    m_pressureSlider->setMinValue(PressureMinValue);

    m_mainGroup->appendItem(m_pressureSlider);
    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(m_pressureSlider, &WacomPressure::requestSetSliderValue, this, &WacomSettings::requestSetPressureValue);
}

void WacomSettings::setModel(WacomModelBase *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &WacomModelBase::pressureValueChanged, this, &WacomSettings::setPressureValue);
    setPressureValue(m_baseSettings->getPressureValue());
}

void WacomSettings::setPressureValue(const int &value)
{
    m_pressureSlider->setValue(value);
}
