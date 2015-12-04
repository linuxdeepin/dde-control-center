#ifndef NETWORKGENERICLISTITEM_H
#define NETWORKGENERICLISTITEM_H

#include "genericlistitem.h"
#include "networkglobal.h"

class DBusNetwork;
class NetworkGenericListItem : public GenericListItem
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString connectPath READ connectPath WRITE setConnectPath NOTIFY connectPathChanged)
    Q_PROPERTY(bool secured READ secured WRITE setSecured NOTIFY securedChanged)
    Q_PROPERTY(bool securedInEap READ securedInEap WRITE setSecuredInEap NOTIFY securedInEapChanged)
    Q_PROPERTY(int strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString devicePath READ devicePath WRITE setDevicePath NOTIFY devicePathChanged)

public:
    explicit NetworkGenericListItem(DBusNetwork *dbus, QWidget *parent = 0);

    QString path() const;
    QString ssid() const;
    QString uuid() const;
    QString connectPath() const;
    bool secured() const;
    bool securedInEap() const;
    int strength() const;
    int state() const;
    QString devicePath() const;

    void updateInfoByMap(const QVariantMap &map);

public slots:
    void setPath(QString path);
    void setSsid(QString ssid);
    void setUuid(QString uuid);
    void setConnectPath(QString connectPath);
    void setSecured(bool secured);
    void setSecuredInEap(bool securedInEap);
    void setStrength(int strength);
    void setState(int state);
    void setDevicePath(QString devicePath);
    void onArrowClicked();

signals:
    void uuidChanged(QString uuid);
    void connectPathChanged(QString connectPath);
    void securedChanged(bool secured);
    void securedInEapChanged(bool securedInEap);
    void strengthChanged(int strength);
    void stateChanged(int state);
    void devicePathChanged(QString devicePath);
    void rightArrowClicked();

private:
    DBusNetwork *m_dbusNetwork;
    QString m_path;
    QString m_uuid;
    bool m_secured = false;
    bool m_securedInEap = true;
    int m_strength = -1;
    int m_state = ActiveConnectionState::Unknown;
    QString m_connectPath;
    QString m_devicePath;
};

#endif // NETWORKGENERICLISTITEM_H
