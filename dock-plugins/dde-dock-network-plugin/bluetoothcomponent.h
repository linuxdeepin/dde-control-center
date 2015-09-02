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
class QFrame;
class BluetoothComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    BluetoothComponent(QString id, QObject *parent = 0);
    ~BluetoothComponent();

    QPixmap getIcon() Q_DECL_OVERRIDE;
    QString getName() Q_DECL_OVERRIDE;
    QString getTitle() Q_DECL_OVERRIDE;
    QString getCommand() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QLabel * m_item;
    QFrame * m_applet;
    QList<BluetoothAdaptor> m_adaptors;
    QString m_id;

    com::deepin::daemon::Bluetooth * m_dbusBluetooth;

    void updateApplet();
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
