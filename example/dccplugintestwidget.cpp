#include "dccplugintestwidget.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "dccnetworkmodule.h"

DccPluginTestWidget::DccPluginTestWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pModule = new DCCNetworkModule;
    m_pModule->setFrameProxy(this);

    m_button = new QPushButton(this);
    m_button->setText(tr("network"));

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    m_topWidget = new QWidget(this);

    QHBoxLayout *pTopLayout = new QHBoxLayout(this);
    m_topWidget->setLayout(pTopLayout);
    pTopLayout->setContentsMargins(0, 0, 0, 0);
    pTopLayout->setSpacing(0);
    pTopLayout->addWidget(m_button);
    pTopLayout->addStretch();

    m_bottomWidget = new QWidget(this);
    QHBoxLayout *pBottomLayout = new QHBoxLayout(this);
    m_bottomWidget->setLayout(pBottomLayout);
    pBottomLayout->addWidget(m_pModule->moduleWidget());

    pLayout->addWidget(m_topWidget);
    pLayout->addWidget(m_bottomWidget);

    m_topWidget->setFixedHeight(30);

    connect(m_button, &QPushButton::clicked, [ = ] () {
        bool visible = m_pModule->moduleWidget()->isVisible();
        m_pModule->moduleWidget()->setVisible(!visible);
    });
}

DccPluginTestWidget::~DccPluginTestWidget()
{
    delete m_pModule;
}

void DccPluginTestWidget::pushWidget(ModuleInterface * const inter, QWidget * const w, FrameProxyInterface::PushType type)
{
    Q_UNUSED(inter);
    Q_UNUSED(w);
    Q_UNUSED(type);
}

void DccPluginTestWidget::popWidget(ModuleInterface * const inter)
{
    Q_UNUSED(inter);
}

void DccPluginTestWidget::setModuleVisible(ModuleInterface * const inter, const bool visible)
{
    Q_UNUSED(inter);
    Q_UNUSED(visible);
}

void DccPluginTestWidget::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_UNUSED(module);
    Q_UNUSED(page);
    Q_UNUSED(animation);
}

void DccPluginTestWidget::setModuleSubscriptVisible(const QString &module, bool bIsDisplay)
{
    Q_UNUSED(module);
    Q_UNUSED(bIsDisplay);
}

void DccPluginTestWidget::setRemoveableDeviceStatus(QString type, bool state)
{
    Q_UNUSED(type);
    Q_UNUSED(state);
}

bool DccPluginTestWidget::getRemoveableDeviceStatus(QString type) const
{
    Q_UNUSED(type);
    return true;
}

void DccPluginTestWidget::setSearchPath(ModuleInterface * const inter) const
{
    Q_UNUSED(inter);
}
