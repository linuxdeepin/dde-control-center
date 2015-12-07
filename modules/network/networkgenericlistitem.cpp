#include <QSvgWidget>

#include <libdui/dthememanager.h>

#include "imagenamebutton.h"
#include "networkgenericlistitem.h"
#include "abstractdevicewidget.h"
#include "networkmainwidget.h"
#include "editconnectionpage.h"

NetworkGenericListItem::NetworkGenericListItem(DBusNetwork *dbus, QWidget *parent) :
    GenericListItem(parent),
    m_dbusNetwork(dbus)
{
    QSvgWidget *strength_icon = new QSvgWidget;

    this->addWidgetToRight(strength_icon);

    auto updateStrengthIcon = [this, strength_icon]{
        int strength = this->strength() <= 5
                ? 0 : (this->strength() / 25 + int(this->strength() % 25 > 0)) * 25;

        const QString icon_path = ":/" + DThemeManager::instance()->theme()
                + "/images/wifi-signal-" + QString::number(strength)
                + (secured() ? "-secure.svg" : ".svg");

        strength_icon->load(icon_path);
        strength_icon->setFixedSize(strength_icon->sizeHint());
    };

    connect(this, &NetworkGenericListItem::strengthChanged, this, updateStrengthIcon);
    connect(this, &NetworkGenericListItem::securedChanged, this, updateStrengthIcon);
    connect(this, &NetworkGenericListItem::mouseEnter, this, [this] {
        if(loading() || checked()) {
            setShowClearButton(true);
        }
    });
    connect(this, &NetworkGenericListItem::mouseLeave, this, [this] {
        setShowClearButton(false);
    });

    ImageNameButton *arrow_button = new ImageNameButton("arrow_right");

    arrow_button->setFixedSize(arrow_button->sizeHint());

    rightLayout()->addWidget(arrow_button);
    rightLayout()->addSpacing(10);

    connect(arrow_button, &ImageNameButton::clicked, this, &NetworkGenericListItem::rightArrowClicked);
    connect(this, SIGNAL(rightArrowClicked()), SLOT(onArrowClicked()));
    /// 改动此处代码时需注意: 在WirelessNetworkListItem中调用了disconnect此连接，记得一起改。
}

QString NetworkGenericListItem::path() const
{
    return m_path;
}

QString NetworkGenericListItem::ssid() const
{
    return title();
}

QString NetworkGenericListItem::uuid() const
{
    return m_uuid;
}

QString NetworkGenericListItem::connectPath() const
{
    return m_connectPath;
}

bool NetworkGenericListItem::secured() const
{
    return m_secured;
}

bool NetworkGenericListItem::securedInEap() const
{
    return m_securedInEap;
}

int NetworkGenericListItem::strength() const
{
    return m_strength;
}

int NetworkGenericListItem::state() const
{
    return m_state;
}

QString NetworkGenericListItem::devicePath() const
{
    return m_devicePath;
}

void NetworkGenericListItem::updateInfoByMap(const QVariantMap &map)
{
    for(QString str : map.keys()) {
        const QVariant &value = map.value(str);
        str[0] = str[0].toLower();
        setProperty(qPrintable(str), value);
    }
}

void NetworkGenericListItem::setPath(QString path)
{
    if (m_path == path)
        return;

    m_path = path;
}

void NetworkGenericListItem::setSsid(QString ssid)
{
    setTitle(ssid);
}

void NetworkGenericListItem::setUuid(QString uuid)
{
    if (m_uuid == uuid)
        return;

    m_uuid = uuid;
    emit uuidChanged(uuid);
}

void NetworkGenericListItem::setConnectPath(QString connectPath)
{
    if (m_connectPath == connectPath)
        return;

    m_connectPath = connectPath;
    emit connectPathChanged(connectPath);
}

void NetworkGenericListItem::setSecured(bool secured)
{
    if (m_secured == secured)
        return;

    m_secured = secured;
    emit securedChanged(secured);
}

void NetworkGenericListItem::setSecuredInEap(bool securedInEap)
{
    if (m_securedInEap == securedInEap)
        return;

    m_securedInEap = securedInEap;
    emit securedInEapChanged(securedInEap);
}

void NetworkGenericListItem::setStrength(int strength)
{
    if (m_strength == strength)
        return;

    m_strength = strength;
    emit strengthChanged(strength);
}

void NetworkGenericListItem::setState(int state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(state);
}

void NetworkGenericListItem::setDevicePath(QString devicePath)
{
    if (m_devicePath == devicePath)
        return;

    m_devicePath = devicePath;
    emit devicePathChanged(devicePath);
}

void NetworkGenericListItem::onArrowClicked()
{
    qDebug() << "ArrowClicked clicked";

    if(uuid().isEmpty() || devicePath().isEmpty())
        return;

    NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

    if(main_widget) {
        const QDBusObjectPath &path = m_dbusNetwork->EditConnection(uuid(), QDBusObjectPath(devicePath()));

        if(path.path().isEmpty())
            return;

        main_widget->pushWidget(new EditConnectionPage(path.path()));
    }
}

