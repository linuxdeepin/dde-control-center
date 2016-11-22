#include "mousesettings.h"

MouseSettings::MouseSettings(const QString &title, QObject *parent)
{
    Q_UNUSED(parent);
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    speedSlider = new SpeedSlider;
    m_title    = new ModuleTitle(title);
    speedSlider->setTitle(tr("Pointer Speed"));
    speedSlider->setMaxValue(MouseMoveSpeedMax);
    speedSlider->setMinValue(MouseMoveSpeedMin);
    speedSlider->setStep(MouseMoveSpeedStep);

    m_mainGroup->appendItem(m_title);
    m_mainGroup->appendItem(speedSlider);

    switchWidget = new SwitchWidget;
    m_mainGroup->appendItem(switchWidget);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(switchWidget, &SwitchWidget::checkedChanegd, this, &MouseSettings::requestSetSwitch);
    connect(speedSlider, &SpeedSlider::requestSetSliderValue, this, &MouseSettings::requestSetSliderValue);
    setObjectName("MouseSettings");
}

void MouseSettings::setModel(MouseModelMouseSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelMouseSettings::switchChanged, this, &MouseSettings::setSwitchState);
    connect(m_baseSettings, &MouseModelMouseSettings::sliderValueChanged, this, &MouseSettings::setSliderValue);
    setSwitchState(m_baseSettings->getSwitchState());
    setSliderValue(m_baseSettings->getSliderValue());
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
