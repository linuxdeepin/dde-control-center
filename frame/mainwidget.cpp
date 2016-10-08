#include "mainwidget.h"

MainWidget::MainWidget(Frame *parent)
    : FrameWidget(parent),

      m_pluginsController(new PluginsController(this)),

      m_lastPluginWidget(nullptr),

      m_pluginsLayout(new QHBoxLayout),
      m_nextPluginBtn(new QPushButton),
      m_allSettingsBtn(new QPushButton)
{
    m_nextPluginBtn->setText("Next");
    m_allSettingsBtn->setText("All settings");

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(m_pluginsLayout);
    centeralLayout->addWidget(m_nextPluginBtn);
    centeralLayout->addWidget(m_allSettingsBtn);

    setLayout(centeralLayout);

    connect(m_pluginsController, &PluginsController::pluginAdded, this, &MainWidget::pluginAdded);
    connect(m_nextPluginBtn, &QPushButton::clicked, this, &MainWidget::showNextPlugin);
    connect(m_allSettingsBtn, &QPushButton::clicked, this, &MainWidget::showAllSettings);

    m_pluginsController->loadPlugins();
}

void MainWidget::showPlugin(QWidget * const w)
{
    if (m_lastPluginWidget)
        m_lastPluginWidget->setVisible(false);
    m_lastPluginWidget = w;
    m_lastPluginWidget->setVisible(true);
}

void MainWidget::pluginAdded(QWidget * const w)
{
    m_pluginsLayout->addWidget(w);
    showPlugin(w);
}

void MainWidget::showNextPlugin()
{
    const int index = m_pluginsLayout->indexOf(m_lastPluginWidget);
    QLayoutItem *item = m_pluginsLayout->itemAt(index + 1);
    if (item && item->widget())
        showPlugin(item->widget());
    else
        showPlugin(m_pluginsLayout->itemAt(0)->widget());
}
