#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>

#include <QDBusConnection>

#include "powerplugin.h"


PowerPlugin::PowerPlugin()
{
    QIcon::setThemeName("Deepin");

    m_id = "id_power";

    m_label = new QLabel;
    m_label->adjustSize();

    m_dbusPower = new com::deepin::daemon::Power("com.deepin.daemon.Power",
                                                 "/com/deepin/daemon/Power",
                                                 QDBusConnection::sessionBus(),
                                                 this);
    connect(m_dbusPower, &Power::BatteryPercentageChanged, this, &PowerPlugin::updateIcon);
    connect(m_dbusPower, &Power::OnBatteryChanged, this, &PowerPlugin::updateIcon);
}

PowerPlugin::~PowerPlugin()
{

}

void PowerPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_proxy->itemAddedEvent(m_id);

    setMode(proxy->dockMode());
}

QString PowerPlugin::getPluginName()
{
    return "Power plugin";
}

QStringList PowerPlugin::ids()
{
    QStringList list(m_id);

    return list;
}

QString PowerPlugin::getName(QString id)
{
    return getPluginName();
}

QString PowerPlugin::getTitle(QString)
{
    QString batteryPercentage = QString("%1%").arg(QString::number(m_dbusPower->batteryPercentage()));

    if (!m_dbusPower->onBattery()) {
        return "Charging " + batteryPercentage;
    } else {
        return batteryPercentage;
    }
}

QString PowerPlugin::getCommand(QString id)
{
    return "dde-control-center power";
}

bool PowerPlugin::canDisable(QString id)
{
    return true;
}

bool PowerPlugin::isDisabled(QString id)
{
    return false;
}

void PowerPlugin::setDisabled(QString id, bool disabled)
{
    m_label->setParent(NULL);
    m_proxy->itemRemovedEvent(id);
}

QWidget * PowerPlugin::getApplet(QString)
{
    return NULL;
}

QWidget * PowerPlugin::getItem(QString)
{
    if (m_dbusPower->batteryIsPresent()) {
        return m_label;
    } else {
        return NULL;
    }
}

void PowerPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (newMode != oldMode) setMode(newMode);
}

QString PowerPlugin::getMenuContent(QString id)
{
    return "";
}

void PowerPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{

}


// private methods
void PowerPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    updateIcon();
}

void PowerPlugin::updateIcon()
{
    bool batteryPresent = m_dbusPower->batteryIsPresent();

    if (batteryPresent) {
        int batteryPercentage = m_dbusPower->batteryPercentage();

        QIcon fallback = QIcon::fromTheme("application-default-icon");

        if (m_mode == Dock::FashionMode) {
            QString iconName = getBatteryIcon(batteryPercentage, !m_dbusPower->onBattery());
            QIcon icon = QIcon::fromTheme(iconName, fallback);
            m_label->setFixedSize(Dock::APPLET_FASHION_ICON_SIZE, Dock::APPLET_FASHION_ICON_SIZE);
            m_label->setPixmap(icon.pixmap(m_label->size()));
            m_proxy->itemSizeChangedEvent(m_id);
        } else {
            QString iconName = getBatteryIcon(batteryPercentage, !m_dbusPower->onBattery(), true);
            QIcon icon = QIcon::fromTheme(iconName, fallback);
            m_label->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);
            m_label->setPixmap(icon.pixmap(m_label->size()));
            m_proxy->itemSizeChangedEvent(m_id);
        }
    }
}

QString PowerPlugin::getBatteryIcon(int percentage, bool plugged, bool symbolic)
{
    QString percentageStr;

    if (percentage < 20) {
        percentageStr = "000";
    } else if (percentage < 40) {
        percentageStr = "020";
    } else if (percentage < 60) {
        percentageStr = "040";
    } else if (percentage < 80) {
        percentageStr = "060";
    } else if (percentage < 100) {
        percentageStr = "080";
    } else {
        percentageStr = "100";
    }

    if (symbolic) {
        if (plugged) {
            return "battery-charged-symbolic";
        } else {
            return QString("battery-%1-symbolic").arg(percentageStr);
        }
    } else {
        if (plugged) {
            return QString("battery-%1-plugged").arg(percentageStr);
        } else {
            return QString("battery-%1").arg(percentageStr);
        }
    }
}
