#include "defappwidget.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include <QPushButton>
#include <QDebug>

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;
DefaultAppsWidget::DefaultAppsWidget()
    : ModuleWidget(),

    m_userGroup(new SettingsGroup)
{
    setObjectName("Defapp");

    m_centralLayout->addWidget(m_userGroup);

    m_nextPage = new NextPageWidget;
    m_nextPage->setTitle(tr("Default Applications"));
    m_userGroup->appendItem(m_nextPage);
    m_centralLayout->addStretch(0);

    setTitle(tr("Default Applications"));
    connect(m_nextPage, &NextPageWidget::clicked, this, &DefaultAppsWidget::showDefaultAppsDetail);
}
