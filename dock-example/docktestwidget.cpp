#include "docktestwidget.h"
#include "networkplugin.h"

#include <QHBoxLayout>

DockTestWidget::DockTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_networkPlugin(new NetworkPlugin(this))
{
    initDock();
}

DockTestWidget::~DockTestWidget()
{
}

void DockTestWidget::initDock()
{
    m_networkPlugin->init(this);
    QWidget *pluginWidget = m_networkPlugin->itemWidget(NETWORK_KEY);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->addWidget(pluginWidget);
    layout->addStretch();
    pluginWidget->installEventFilter(this);
}

bool DockTestWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object != m_networkPlugin->itemWidget(NETWORK_KEY))
        return QWidget::eventFilter(object, event);

    QWidget *tipWidget = m_networkPlugin->itemTipsWidget(NETWORK_KEY);
    if (!tipWidget)
        return QWidget::eventFilter(object, event);

    if (event->type() == QEvent::Enter) {
        tipWidget->show();
        return true;
    }
    if (event->type() == QEvent::Leave) {
        tipWidget->hide();
        return true;
    }

    return QWidget::eventFilter(object, event);
}

void DockTestWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    QWidget *tip = m_networkPlugin->itemTipsWidget(NETWORK_KEY);
    if (tip) {
        tip->setFixedSize(200, 300);
        tip->show();
    }
}

void DockTestWidget::itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::requestWindowAutoHide(PluginsItemInterface * const itemInter, const QString &itemKey, const bool autoHide)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
    Q_UNUSED(autoHide);
}

void DockTestWidget::requestRefreshWindowVisible(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::requestSetAppletVisible(PluginsItemInterface * const itemInter, const QString &itemKey, const bool visible)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
    Q_UNUSED(visible);
}

void DockTestWidget::saveValue(PluginsItemInterface * const itemInter, const QString &key, const QVariant &value)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(key);
    Q_UNUSED(value);
}

const QVariant DockTestWidget::getValue(PluginsItemInterface * const itemInter, const QString &key, const QVariant &fallback)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(key);
    Q_UNUSED(fallback);
    if (key == "enabled")
        return true;

    return QVariant();
}

void DockTestWidget::removeValue(PluginsItemInterface * const itemInter, const QStringList &keyList)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(keyList);
}
