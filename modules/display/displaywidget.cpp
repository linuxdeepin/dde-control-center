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
      m_mySettings(new NextPageWidget),
      m_settingsGrp(new SettingsGroup),
      m_rotate(new QPushButton),
      m_custom(new QPushButton)
{
    setObjectName("Display");

    m_rotate->setText(tr("Rotate"));
    m_custom->setText(tr("Custom"));

    m_resolution->setTitle(tr("Resolution"));
    m_mySettings->setTitle(tr("My Settings"));

    m_resolutionsGrp = new SettingsGroup;
    m_resolutionsGrp->appendItem(m_resolution);

    m_settingsGrp = new SettingsGroup;
    m_settingsGrp->appendItem(m_mySettings);

    m_centralLayout->addWidget(m_resolutionsGrp);
    m_centralLayout->addWidget(m_rotate);
    m_centralLayout->addWidget(m_settingsGrp);
    m_centralLayout->addWidget(m_custom);

    setTitle(tr("Display"));

    connect(m_resolution, &NextPageWidget::clicked, this, &DisplayWidget::showResolutionPage);
    connect(m_rotate, &QPushButton::clicked, this, &DisplayWidget::requestRotate);
    connect(m_custom, &QPushButton::clicked, this, &DisplayWidget::requestCustom);
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onScreenListChanged);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &DisplayWidget::onScreenSizeChanged);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &DisplayWidget::onScreenSizeChanged);

    onScreenListChanged();
    onScreenSizeChanged();
}

void DisplayWidget::onScreenListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1)
    {
        m_custom->hide();
        m_settingsGrp->hide();

        m_resolutionsGrp->show();
        m_resolution->show();
        m_rotate->show();
    } else {
        m_custom->show();
        // TODO:
        m_settingsGrp->hide();
//        m_settingsGrp->show();

        m_resolutionsGrp->hide();
        m_resolution->hide();
        m_rotate->hide();
    }
}

void DisplayWidget::onScreenSizeChanged() const
{
    const QString resolution = QString("%1×%2").arg(m_model->screenWidth()).arg(m_model->screenHeight());
    m_resolution->setValue(resolution);
}
