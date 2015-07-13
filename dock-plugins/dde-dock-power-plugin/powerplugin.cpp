#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>

#include "powerplugin.h"

PowerPlugin::PowerPlugin()
{
    QIcon::setThemeName("Deepin");

    m_uuid = QUuid::createUuid().toString();

    m_label = new QLabel;
}

PowerPlugin::~PowerPlugin()
{
    m_label->deleteLater();
}

void PowerPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    setMode(proxy->dockMode());

    m_proxy->itemAddedEvent(m_uuid);
}

QString PowerPlugin::name()
{
    return "Power plugin";
}

QStringList PowerPlugin::uuids()
{
    QStringList list(m_uuid);

    return list;
}

QString PowerPlugin::getTitle(QString)
{
    return "100%";
}

QWidget * PowerPlugin::getApplet(QString)
{
    return NULL;
}

QWidget * PowerPlugin::getItem(QString)
{
    return m_label;
}

void PowerPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    qDebug() << "changemode" << "power";

    if (newMode != oldMode) setMode(newMode);
}


// private methods
void PowerPlugin::setMode(Dock::DockMode mode)
{
    QIcon fallback = QIcon::fromTheme("application-default-icon");

    if (mode == Dock::FashionMode) {
        QIcon icon = QIcon::fromTheme("battery-000", fallback);
        m_label->setFixedSize(48, 48);
        m_label->setPixmap(icon.pixmap(m_label->size()));
        m_proxy->itemSizeChangedEvent(m_uuid);
    } else {
        QIcon icon = QIcon::fromTheme("battery-000-symbolic", fallback);
        m_label->setFixedSize(16, 16);
        m_label->setPixmap(icon.pixmap(m_label->size()));
        m_proxy->itemSizeChangedEvent(m_uuid);
    }
}
