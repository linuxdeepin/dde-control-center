#include "mousesettings.h"

MouseSettings::MouseSettings(QObject *parent)
{
    Q_UNUSED(parent);
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    speedSlider = new SpeedSlider;
    speedSlider->setTitle(tr("Pointer Speed"));
    m_mainGroup->appendItem(speedSlider);

    switchWidget = new SwitchWidget;
    m_mainGroup->appendItem(switchWidget);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(switchWidget, &SwitchWidget::checkedChanegd, this, &MouseSettings::requestSetSwitch);
    connect(speedSlider, &SpeedSlider::requestSetSliderValue, this, &MouseSettings::requestSetSliderValue);

}

void MouseSettings::setModel(MouseModelMouseSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelMouseSettings::switchChanged, this, &MouseSettings::setSwitchState);
    connect(m_baseSettings, &MouseModelMouseSettings::sliderValueChanged, this, &MouseSettings::setSliderValue);
    setSwitchState(m_baseSettings->getSwitchState());
    setSliderValue(m_baseSettings->getSliderValue());
    setSliderMaxValue(m_baseSettings->getSliderMaxValue());
    setSliderMinValue(m_baseSettings->getSliderMinValue());
    setSliderStep(m_baseSettings->getSliderStep());
}

void MouseSettings::setSwitchTitle(const QString &title)
{
    switchWidget->setTitle(title);
}

void MouseSettings::setSwitchState(const bool state)
{
    switchWidget->setChecked(state);
}

void MouseSettings::setSliderValue(const int &value)
{
    speedSlider->setValue(value);
}

void MouseSettings::setSliderMaxValue(const int &value)
{
    speedSlider->setMaxValue(value);
}

void MouseSettings::setSliderMinValue(const int &value)
{
    speedSlider->setMinValue(value);
}

void MouseSettings::setSliderStep(const int &value)
{
    speedSlider->setStep(value);
}
