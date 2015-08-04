#include "grubdbus.h"

#define SERVERPATH "com.deepin.daemon.Grub2"
#define INTERFACEPATH "/com/deepin/daemon/Grub2/Theme"
#define INTERFACENAME "com.deepin.daemon.Grub2.Theme"
#define INTERFACEPROPERTYS "org.freedesktop.DBus.Properties"

class GrubDbusPrivate : public GrubDbus
{

};

Q_GLOBAL_STATIC(GrubDbusPrivate, GrubDbusStatic)

GrubDbus *GrubDbus::instance()
{
    return GrubDbusStatic;
}

GrubDbus::GrubDbus(QObject *parent) :
    QDBusAbstractInterface(SERVERPATH,
                           INTERFACEPATH,
                           INTERFACENAME,
                           QDBusConnection::sessionBus(),
                           parent),
    m_updating(false),
    m_timeout(0)
{
    QDBusInterface tmp(SERVERPATH, INTERFACEPATH, INTERFACEPROPERTYS, QDBusConnection::sessionBus());
    QDBusMessage tmp_mess = tmp.call("GetAll", INTERFACENAME);
    QVariantMap changedProps = qdbus_cast<QVariantMap>(tmp_mess.arguments().first().value<QDBusArgument>());
    updatePropertys(changedProps);

    QDBusConnection::sessionBus().connect(service(), path(),
                                          INTERFACEPROPERTYS,
                                          "PropertiesChanged", "sa{sv}as",
                                          this, SLOT(dbusPropertyChanged(QDBusMessage)));
}

GrubDbus::~GrubDbus()
{
    QDBusConnection::sessionBus().disconnect(service(), path(),
                                             INTERFACEPROPERTYS,
                                             "PropertiesChanged", "sa{sv}as",
                                             this, SLOT(dbusPropertyChanged(QDBusMessage)));
}

bool GrubDbus::updating() const
{
    return m_updating;
}

QString GrubDbus::background() const
{
    return m_background;
}

QString GrubDbus::itemColor() const
{
    return m_itemColor;
}

QString GrubDbus::selectedItemColor() const
{
    return m_selectedItemColor;
}

qint32 GrubDbus::timeout() const
{
    return m_timeout;
}

void GrubDbus::setUpdating(bool updating)
{
    if (m_updating == updating)
        return;

    m_updating = updating;
    emit updatingChanged(updating);
}

void GrubDbus::setBackground(QString background)
{
    if (m_background == background)
        return;

    m_background = background;
    emit backgroundChanged(background);
}

void GrubDbus::setItemColor(QString itemColor)
{
    if (m_itemColor == itemColor)
        return;

    m_itemColor = itemColor;
    emit itemColorChanged(itemColor);
}

void GrubDbus::setSelectedItemColor(QString selectedItemColor)
{
    if (m_selectedItemColor == selectedItemColor)
        return;

    m_selectedItemColor = selectedItemColor;
    emit selectedItemColorChanged(selectedItemColor);
}

void GrubDbus::setTimeout(qint32 timeout)
{
    if (m_timeout == timeout)
        return;

    m_timeout = timeout;
    emit timeoutChanged(timeout);
}

void GrubDbus::dbusPropertyChanged(const QDBusMessage &msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count())
        return;
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName != INTERFACENAME){
        qDebug()<<"unknow interface name:"<<interfaceName;
        return;
    }
    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    updatePropertys(changedProps);
}

void GrubDbus::updatePropertys(const QVariantMap &changedProps)
{
    const QMetaObject* self = this->metaObject();

    foreach(const QString &prop, changedProps.keys()) {
        for (int i=self->propertyOffset(); i < self->propertyCount(); ++i) {
            QMetaProperty p = self->property(i);
            QString p_name(p.name());

            if (p_name.toUpper() == prop.toUpper()) {
                QByteArray set_p_name = QByteArray("set").append(p_name[0].toUpper()).append(p_name.mid(1));
                QVariant value = changedProps[prop];
                self->invokeMethod(this, set_p_name.data(), QGenericArgument(value.typeName(), value.data()));
                Q_EMIT p.notifySignal().invoke(this);
                break;
            }
        }
    }
}

