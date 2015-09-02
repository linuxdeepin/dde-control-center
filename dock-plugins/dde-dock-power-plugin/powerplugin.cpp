#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>

#include <QDBusConnection>

#include "powerplugin.h"

PowerPlugin::PowerPlugin()
{
    QIcon::setThemeName("Deepin");

    m_id = "id_power";

    m_label = new QLabel;
    m_label->adjustSize();

    m_dbusPower = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                     "/com/deepin/daemon/Power",
                                                     QDBusConnection::sessionBus(),
                                                     this);
    connect(m_dbusPower, &DBusPower::BatteryPercentageChanged, this, &PowerPlugin::updateIcon);
    connect(m_dbusPower, &DBusPower::OnBatteryChanged, this, &PowerPlugin::updateIcon);

    this->initSettings();
}

PowerPlugin::~PowerPlugin()
{

}

void PowerPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    setMode(proxy->dockMode());

    onDisableChanged();
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

QString PowerPlugin::getName(QString)
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

QString PowerPlugin::getCommand(QString)
{
    return "dde-control-center power";
}

QPixmap PowerPlugin::getIcon(QString)
{
    QString iconName = getBatteryIcon(100, false, true);
    QIcon icon = QIcon::fromTheme(iconName, QIcon::fromTheme("application-default-icon"));
    return icon.pixmap(QSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE));
}

bool PowerPlugin::canDisable(QString)
{
    return m_dbusPower->batteryIsPresent();
}

bool PowerPlugin::isDisabled(QString)
{
    return m_settings->value(settingDisabledKey()).toBool();
}

void PowerPlugin::setDisabled(QString id, bool disabled)
{
    if (id != m_id)
        return;

    m_settings->setValue(settingDisabledKey(), disabled);

    onDisableChanged();
}

QWidget * PowerPlugin::getApplet(QString)
{
    return NULL;
}

QWidget * PowerPlugin::getItem(QString)
{
    bool disable = m_settings->value(settingDisabledKey()).toBool();

    if (m_dbusPower->batteryIsPresent() && !disable) {
        return m_label;
    } else {
        return NULL;
    }
}

void PowerPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (newMode != oldMode){
        //make sure m_mode changed beford update disable-state
        setMode(newMode);
        onDisableChanged();
    }
}

QString PowerPlugin::getMenuContent(QString)
{
    return "";
}

void PowerPlugin::invokeMenuItem(QString, QString, bool)
{

}


// private methods
void PowerPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-power-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        m_settings->setValue(settingDisabledKey(), true);
    }
}

void PowerPlugin::onDisableChanged()
{
    m_proxy->itemRemovedEvent(m_id);
    m_label->setParent(NULL);

    if (!isDisabled(m_id))
        m_proxy->itemAddedEvent(m_id);
}

void PowerPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    updateIcon();
}

QString PowerPlugin::settingDisabledKey()
{
    return QString::number(m_mode) + "/disabled";
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
