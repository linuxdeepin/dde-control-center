#include "displaywidget.h"
#include "translucentframe.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "displaymodel.h"

using namespace dcc::widgets;
using namespace dcc::display;

DisplayWidget::DisplayWidget()
    : ModuleWidget(),

      m_resolution(new NextPageWidget),
      m_rotate(new QPushButton),
      m_custom(new QPushButton)
{
    setObjectName("Display");

    m_rotate->setText(tr("Rotate"));
    m_custom->setText(tr("Custom"));

    m_resolution->setTitle(tr("Resolution"));

    SettingsGroup *resolutions = new SettingsGroup;
    resolutions->appendItem(m_resolution);

    m_centeralLayout->addWidget(resolutions);
    m_centeralLayout->addWidget(m_rotate);
    m_centeralLayout->addWidget(m_custom);

    setTitle(tr("Display"));

    connect(m_resolution, &NextPageWidget::clicked, this, &DisplayWidget::showResolutionPage);
    connect(m_rotate, &QPushButton::clicked, this, &DisplayWidget::requestRotate);
    connect(m_custom, &QPushButton::clicked, this, &DisplayWidget::requestCustom);
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onScreenListChanged);

    onScreenListChanged();
}

void DisplayWidget::onScreenListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1)
    {
        m_custom->hide();
        m_resolution->show();
        m_rotate->show();
    } else {
        m_custom->show();
        m_resolution->hide();
        m_rotate->hide();
    }

    const QString resolution = QString("%1×%2").arg(m_model->screenWidth()).arg(m_model->screenHeight());
    m_resolution->setValue(resolution);
}
