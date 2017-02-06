#include "wacomwidget.h"

#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "wacom/wacommodel.h"
#include "wacom/widget/wacomsettings.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::wacom;
WacomWidget::WacomWidget()
    : ModuleWidget(),

      m_userGroup(new SettingsGroup)
{
    setObjectName("Wacom");

    m_centralLayout->addWidget(m_userGroup);
    m_wacomSettings = new WacomSettings;
    m_centralLayout->addWidget(m_wacomSettings);

    setTitle(tr("Wacom"));
    connect(m_wacomSettings, &WacomSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
}

void WacomWidget::setModel(WacomModel *const model)
{
    m_wacomSettings->setModel(model->getWacomModelBase());
}
