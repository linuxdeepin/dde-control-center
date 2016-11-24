#include "displaywidget.h"
#include "translucentframe.h"

using namespace dcc;
using namespace dcc::display;

DisplayWidget::DisplayWidget()
    : ModuleWidget(),

      m_resolution(new NextPageWidget),
      m_rotate(new QPushButton),
      m_custom(new QPushButton)
{
    m_rotate->setText(tr("Rotate"));
    m_custom->setText(tr("Custom"));

    SettingsGroup *resolutions = new SettingsGroup;
    resolutions->appendItem(m_resolution);

    m_centeralLayout->addWidget(resolutions);
    m_centeralLayout->addWidget(m_rotate);
    m_centeralLayout->addWidget(m_custom);

    setTitle(tr("Display"));
}
