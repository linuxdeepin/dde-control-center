#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <QTimer>

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

    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(onInitTimerTriggered()));
    initTimer->start(1000);

    this->initSettings();
}

PowerPlugin::~PowerPlugin()
{

}

void PowerPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
}

QString PowerPlugin::getPluginName()
{
    return tr("Power");
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
    if (!m_dbusPower->isValid())
        return getPluginName();

    QString batteryPercentage = QString("%1%").arg(QString::number(m_dbusPower->batteryPercentage()));

    if (!m_dbusPower->onBattery()) {
        return tr("On Charging %1").arg(batteryPercentage);
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
    return m_dbusPower->isValid() ? m_dbusPower->batteryIsPresent() : false;
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
    if (!m_dbusPower->isValid())
        return NULL;

    bool disable = m_settings->value(settingDisabledKey()).toBool();

    if (m_dbusPower->batteryIsPresent() && !disable) {
        return m_label;
    } else {
        return NULL;
    }
}

void PowerPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (m_dbusPower->isValid() && (newMode != oldMode)){
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

void PowerPlugin::onInitTimerTriggered()
{
    QTimer *t = qobject_cast<QTimer *>(sender());

    if (t && m_dbusPower->isValid()) {
        qWarning() << "PowerPlugin: DBus data is ready!";
        t->stop();
        t->deleteLater();

        setMode(m_proxy->dockMode());
        onDisableChanged();
    }
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

    m_proxy->infoChangedEvent(DockPluginInterface::CanDisable, m_id);
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
    if (!m_dbusPower->isValid())
        return;

    bool batteryPresent = m_dbusPower->batteryIsPresent();

    if (batteryPresent) {
        int batteryPercentage = m_dbusPower->batteryPercentage();

        QIcon fallback = QIcon::fromTheme("application-default-icon");

        if (m_mode == Dock::FashionMode) {
            QString iconName = getBatteryIcon(batteryPercentage, !m_dbusPower->onBattery());
            QIcon icon = QIcon::fromTheme(iconName, fallback);
            m_label->setFixedSize(Dock::APPLET_FASHION_ICON_SIZE, Dock::APPLET_FASHION_ICON_SIZE);
            m_label->setPixmap(icon.pixmap(m_label->size()));
            m_proxy->infoChangedEvent(DockPluginInterface::ItemSize, m_id);
        } else {
            QString iconName = getBatteryIcon(batteryPercentage, !m_dbusPower->onBattery(), true);
            QIcon icon = QIcon::fromTheme(iconName, fallback);
            m_label->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);
            m_label->setPixmap(icon.pixmap(m_label->size()));
            m_proxy->infoChangedEvent(DockPluginInterface::ItemSize, m_id);
        }
    }
}

QString PowerPlugin::getBatteryIcon(int percentage, bool plugged, bool symbolic)
{
    QString percentageStr;

    if (percentage < 10) {
        percentageStr = "000";
    } else if (percentage < 30) {
        percentageStr = "020";
    } else if (percentage < 50) {
        percentageStr = "040";
    } else if (percentage < 70) {
        percentageStr = "060";
    } else if (percentage < 90) {
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
