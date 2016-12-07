#include "networkmodulewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"

using namespace dcc;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget(),

      m_detailPage(new NextPageWidget)
{
    m_detailPage->setTitle(tr("Network Details"));

    SettingsGroup *detailGroup = new SettingsGroup;
    detailGroup->appendItem(m_detailPage);

    m_centeralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailPage, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowDetailPage);
}
