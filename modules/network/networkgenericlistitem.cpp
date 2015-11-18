#include <QSvgWidget>

#include <libdui/dthememanager.h>

#include "imagenamebutton.h"
#include "networkgenericlistitem.h"
#include "abstractdevicewidget.h"

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

    rightLayout()->addSpacing(10);
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

