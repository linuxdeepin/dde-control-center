#ifndef BLUETOOTHCOMPONENT_H
#define BLUETOOTHCOMPONENT_H

#include <QObject>
#include <QList>
#include <QJsonObject>

#include "plugincomponentinterface.h"

#include "dbus/dbusbluetooth.h"

struct BluetoothAdaptor
{
    QString path;
    QString alias;
    bool powered;
    bool discovering;
    bool discoverable;
    int discoverableTimeout;
};

class QLabel;
class BluetoothComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    BluetoothComponent(QObject *parent = 0);
    ~BluetoothComponent();

    QString getUUID() Q_DECL_OVERRIDE;

    QString getTitle() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QLabel * m_item;
    QList<BluetoothAdaptor> m_adaptors;

    com::deepin::daemon::Bluetooth * m_dbusBluetooth;

    void updateItem();

    void initAdaptors();
    bool isAnyAdaptorOn();

    void addAdaptor(QJsonObject json);
    void removeAdaptor(QJsonObject json);
    void updateAdaptor(QJsonObject json);

private slots:
    void onAdaptorAdded(QString adaptor);
    void onAdaptorRemoved(QString adaptor);
    void onAdaptorPropertiesChanged(QString adaptor);
};

#endif // BLUETOOTHCOMPONENT_H
