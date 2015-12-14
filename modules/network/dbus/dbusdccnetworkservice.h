#ifndef DBUSDCCNETWORKSERVICE_H
#define DBUSDCCNETWORKSERVICE_H

#include <QDBusAbstractAdaptor>

class NetworkMainWidget;
class DBusDCCNetworkService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.ControlCenter.Network")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"com.deepin.dde.ControlCenter.Network\">\n"
                "    <property access=\"read\" type=\"b\" name=\"active\"/>\n"
                "    <method name=\"active\">\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>"
                "  </interface>\n"
                "")

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    explicit DBusDCCNetworkService(NetworkMainWidget *parent);

    Q_SLOT bool active() const;

signals:
    void activeChanged();

private:
    NetworkMainWidget *m_mainWidget;
};

#endif // DBUSDCCNETWORKSERVICE_H
