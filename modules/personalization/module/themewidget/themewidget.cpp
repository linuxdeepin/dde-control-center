#include "themewidget.h"
#include "settingsgroup.h"
#include "personalization/personalizationmodel.h"
#include "theme.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

ThemeWidget::ThemeWidget(QWidget *parent)
    :ContentWidget(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_widget = new TranslucentFrame;
    m_windowTheme = new Theme(tr("Window Theme"));
    m_iconTheme = new Theme(tr("Icon Theme"));
    m_mouseTheme = new Theme(tr("Cursor Theme"));

    m_mainlayout->addSpacing(10);

    m_mainlayout->addWidget(m_windowTheme);
    m_mainlayout->addWidget(m_iconTheme);
    m_mainlayout->addWidget(m_mouseTheme);

    m_mainlayout->setMargin(0);
    m_mainlayout->setSpacing(20);

    m_widget->setLayout(m_mainlayout);
    setTitle(tr("Theme"));
    setContent(m_widget);

    connect(m_windowTheme, &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
    connect(m_iconTheme,   &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
    connect(m_mouseTheme,  &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
}

void ThemeWidget::setModel(PersonalizationModel * const model)
{
    m_windowTheme->setModel(model->getWindowModel());
    m_iconTheme->setModel(model->getIconModel());
    m_mouseTheme->setModel(model->getMouseModel());
}
