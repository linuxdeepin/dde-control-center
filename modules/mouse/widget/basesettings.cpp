#include "basesettings.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "switchwidget.h"
#include "titledslideritem.h"
#include "../mousemodel.h"
#include "mouse/model/mousemodelbasesettings.h"
#include "doutestwidget.h"
#include "dccslider.h"

#include <QWidget>
#include <QSlider>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::mouse;

BaseSettings::BaseSettings(QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainLayout = new QVBoxLayout;
    m_mainGroup  = new SettingsGroup;
    m_leftHand   = new SwitchWidget;
    m_naturalScroll = new SwitchWidget;
    m_isTyping   = new SwitchWidget;
    m_baseSettings = new MouseModelBaseSettings;
    m_leftHand->setTitle(tr("Left Hand"));
    m_naturalScroll->setTitle(tr("Natural Scrolling"));
    m_isTyping->setTitle(tr("Disable the touchpad while typing"));

    m_mainGroup->appendItem(m_leftHand);
    m_mainGroup->appendItem(m_naturalScroll);
    m_mainGroup->appendItem(m_isTyping);


    QStringList list;
    list << tr("Slow") << "" << "" << "" << "" << "" << tr("Fast");

    douSlider = new TitledSliderItem(tr("Double-click Speed"));
    douSlider->slider()->setType(DCCSlider::Vernier);
    douSlider->slider()->setTickPosition(QSlider::TicksBelow);
    douSlider->slider()->setRange(0, 6);
    douSlider->slider()->setTickInterval(1);
    douSlider->slider()->setPageStep(1);
    douSlider->setAnnotations(list);
    douSlider->slider()->setAccessibleName("Double-click Speed");

    m_mainGroup->appendItem(douSlider);

    DouTestWidget *testWidget = new DouTestWidget;
    m_mainGroup->appendItem(testWidget);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(m_leftHand, &SwitchWidget::checkedChanegd, this, &BaseSettings::requestSetLeftHand);
    connect(m_isTyping, &SwitchWidget::checkedChanegd, this, &BaseSettings::requestSetDisTyping);
    connect(m_naturalScroll, &SwitchWidget::checkedChanegd, this, &BaseSettings::requestSetNaturalScroll);

    QSlider *slider = douSlider->slider();
    connect(slider, &QSlider::valueChanged, this, &BaseSettings::requestSetSliderValue);

    setObjectName("BaseSettings");

}

void BaseSettings::setModel(MouseModelBaseSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelBaseSettings::leftHandStateChanged, this, &BaseSettings::setLeftHandState);
    connect(m_baseSettings, &MouseModelBaseSettings::naturalScrollChanged, this, &BaseSettings::setNaturalScroll);
    connect(m_baseSettings, &MouseModelBaseSettings::disIfTypingStateChanged, this, &BaseSettings::setDisIfTypingState);
    connect(m_baseSettings, &MouseModelBaseSettings::sliderValueChanged, this, &BaseSettings::setSliderValue);

    setLeftHandState(m_baseSettings->getLeftHandState());
    setNaturalScroll(m_baseSettings->getNaturalScroll());
    setDisIfTypingState(m_baseSettings->getDisIfTyping());
    setSliderValue(m_baseSettings->getSliderValue());
}

void BaseSettings::setLeftHandState(const bool state)
{
    blockSignals(true);
    m_leftHand->setChecked(state);
    blockSignals(false);
}

void BaseSettings::setNaturalScroll(const bool state)
{
    blockSignals(true);
    m_naturalScroll->setChecked(state);
    blockSignals(false);
}

void BaseSettings::setDisIfTypingState(const bool state)
{
    blockSignals(true);
    m_isTyping->setChecked(state);
    blockSignals(false);
}

void BaseSettings::setSliderValue(const int &value)
{
    QSlider *slider = douSlider->slider();
    slider->blockSignals(true);
    slider->setValue(value);
    slider->blockSignals(false);
}
