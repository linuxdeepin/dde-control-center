#include "personalizationwidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "personalizationmodel.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

PersonalizationWidget::PersonalizationWidget()
    : ModuleWidget(),

      m_userGroup(new SettingsGroup)
{
    setObjectName("Personalization");

    m_centeralLayout->addWidget(m_userGroup);
    NextPageWidget *theme = new NextPageWidget;
    NextPageWidget *font = new NextPageWidget;

    theme->setTitle(tr("Theme"));
    font->setTitle(tr("Font"));

    m_userGroup->appendItem(theme);
    m_userGroup->appendItem(font);

    setTitle(tr("Personalization"));
    connect(theme, &NextPageWidget::clicked, this, &PersonalizationWidget::showThemeWidget);
    connect(font, &NextPageWidget::clicked, this, &PersonalizationWidget::showFontsWidget);
}
