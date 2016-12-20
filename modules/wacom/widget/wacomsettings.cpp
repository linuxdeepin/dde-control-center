#include "wacomsettings.h"
#include "settingsitem.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "wacom/wacommodel.h"
#include "wacom/model/wacommodelbase.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::wacom;

WacomSettings::WacomSettings(QWidget *parent):
    TranslucentFrame(parent)
{
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    m_pressureSlider = new TitledSliderItem(tr("Pressure Sensitive"));
    m_pressureSlider->slider()->setType(DCCSlider::Vernier);
    m_pressureSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_pressureSlider->slider()->setRange(1, 7);
    m_pressureSlider->slider()->setTickInterval(1);
    m_pressureSlider->slider()->setPageStep(1);

    QStringList delays;
    delays<<tr("light")<<""<<""<<""<<""<<""<<tr("heavy");
    m_pressureSlider->setAnnotations(delays);
    m_mainGroup->appendItem(m_pressureSlider);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    setObjectName("WacomSettings");
    m_preSlider = m_pressureSlider->slider();
    connect(m_preSlider, &QSlider::valueChanged, this, &WacomSettings::requestSetPressureValue);
}

void WacomSettings::setModel(WacomModelBase *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &WacomModelBase::pressureValueChanged, this, &WacomSettings::setPressureValue);
    setPressureValue(m_baseSettings->getPressureValue());
}

void WacomSettings::setPressureValue(const int &value)
{
    m_preSlider->blockSignals(true);
    m_preSlider->setValue(value);
    m_preSlider->blockSignals(false);
}
