#include "wacomwidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;

WacomWidget::WacomWidget()
    : ModuleWidget(),

      m_userGroup(new SettingsGroup)
{
    m_centeralLayout->addWidget(m_userGroup);
    m_wacomSettings = new WacomSettings;
    m_centeralLayout->addWidget(m_wacomSettings);

    setTitle(tr("Wacom"));
    connect(m_wacomSettings, &WacomSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
}

void WacomWidget::setModel(WacomModel *const model)
{
    m_wacomSettings->setModel(model->getWacomModelBase());
}
