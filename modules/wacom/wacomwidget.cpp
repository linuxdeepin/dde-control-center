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
      m_selectMode(new NextPageWidget())
{
    setObjectName("Wacom");

    SettingsGroup *modeGrp = new SettingsGroup;
    modeGrp->appendItem(m_selectMode);
    m_selectMode->setTitle(tr("Mode"));

    m_centralLayout->addWidget(modeGrp);

    m_wacomSettings = new WacomSettings;
    m_centralLayout->addWidget(m_wacomSettings);

    setTitle(tr("Wacom"));
    connect(m_wacomSettings, &WacomSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
    connect(m_selectMode, &NextPageWidget::clicked, this, &WacomWidget::requestShowMode);
}

void WacomWidget::setModel(WacomModel *const model)
{
    m_wacomSettings->setModel(model->getWacomModelBase());

    connect(model, &WacomModel::cursorModeChanged, this, &WacomWidget::onCursorModeChanged);
    onCursorModeChanged(model->getCursorMode());
}

const QString WacomWidget::getValue(const bool mode)
{
    return mode ? tr("Mouse") : tr("Pen");
}

void WacomWidget::onCursorModeChanged(const bool modeChanged)
{
    m_selectMode->setValue(getValue(modeChanged));
    m_wacomSettings->setVisible(!modeChanged);
}
