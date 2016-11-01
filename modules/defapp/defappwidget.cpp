#include "defappwidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;

DefaultAppsWidget::DefaultAppsWidget()
    : ModuleWidget(),

    m_userGroup(new SettingsGroup)
{
    m_centeralLayout->addWidget(m_userGroup);

    m_nextPage = new QPushButton;
    m_nextPage->setText("Set Default Applications >");
    m_centeralLayout->addWidget(m_nextPage);
    m_centeralLayout->addStretch(0);

    setTitle(tr("Default Applications"));
    connect(m_nextPage, &QPushButton::clicked, this, &DefaultAppsWidget::showDefaultAppsDetail);
}
