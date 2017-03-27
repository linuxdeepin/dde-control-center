#include "displaywidget.h"
#include "translucentframe.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "displaymodel.h"
#include "displayworker.h"

using namespace dcc::widgets;
using namespace dcc::display;

DisplayWidget::DisplayWidget()
    : ModuleWidget(),

      m_resolution(new NextPageWidget),
      m_brightnessSettings(new NextPageWidget),
      m_customSettingsGrp(new SettingsGroup),
      m_rotate(new QPushButton),
      m_createConfig(new QPushButton),

      m_configListRefershTimer(new QTimer(this))
{
    setObjectName("Display");

    m_rotate->setText(tr("Rotate"));
    m_createConfig->setText(tr("New custom settings"));

    m_resolution->setTitle(tr("Resolution"));
    m_brightnessSettings->setTitle(tr("Brightness"));

    m_resolutionsGrp = new SettingsGroup;
    m_resolutionsGrp->appendItem(m_resolution);

    m_customSettingsGrp = new SettingsGroup;

    SettingsGroup *brightnessGrp = new SettingsGroup;
    brightnessGrp->appendItem(m_brightnessSettings);

    m_centralLayout->addWidget(m_resolutionsGrp);
    m_centralLayout->addWidget(brightnessGrp);
    m_centralLayout->addWidget(m_rotate);
    m_centralLayout->addWidget(m_customSettingsGrp);
    m_centralLayout->addWidget(m_createConfig);

    m_configListRefershTimer->setSingleShot(true);
    m_configListRefershTimer->setInterval(100);

    setTitle(tr("Display"));

    connect(m_brightnessSettings, &NextPageWidget::clicked, this, &DisplayWidget::showBrightnessPage);
    connect(m_resolution, &NextPageWidget::clicked, this, &DisplayWidget::showResolutionPage);
    connect(m_rotate, &QPushButton::clicked, this, &DisplayWidget::requestRotate);
    connect(m_createConfig, &QPushButton::clicked, this, &DisplayWidget::requestNewConfig);
    connect(m_configListRefershTimer, &QTimer::timeout, this, &DisplayWidget::onConfigListChanged);
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onScreenListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onScreenListChanged);
    connect(m_model, &DisplayModel::configListChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::currentConfigChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::displayModeChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::screenHeightChanged, this, &DisplayWidget::onScreenSizeChanged, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &DisplayWidget::onScreenSizeChanged, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::firstConfigCreated, this, &DisplayWidget::onFirstConfigCreated, Qt::QueuedConnection);

    onScreenListChanged();
    onScreenSizeChanged();
    m_configListRefershTimer->start();
}

void DisplayWidget::onScreenListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1)
    {
        m_createConfig->hide();
        m_customSettingsGrp->hide();

        m_resolutionsGrp->show();
        m_resolution->show();
        m_rotate->show();
    } else {
        m_createConfig->show();
        m_customSettingsGrp->setVisible(!m_model->configList().isEmpty());

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

void DisplayWidget::onConfigListChanged()
{
    m_customSettingsGrp->clear();
    for (auto *w : m_customSettings)
        QTimer::singleShot(1, w, &NextPageWidget::deleteLater);
    m_customSettings.clear();

    const auto mode = m_model->displayMode();
    const auto current = m_model->config();
    const auto configList = m_model->configList();

    for (const auto &config : configList)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(config);
        if (mode == CUSTOM_MODE && config == current)
            w->setIcon(QPixmap(":/widgets/themes/dark/icons/select.png"));

        connect(w, &NextPageWidget::acceptNextPage, this, [=] { emit requestConfigPage(config); });
        connect(w, &NextPageWidget::selected, this, [=] { emit requestSwitchConfig(config); });

        m_customSettingsGrp->appendItem(w);
    }
}

void DisplayWidget::onFirstConfigCreated(const QString &config)
{
    emit requestConfigPage(config);
    emit requestModifyConfig(config);
}
