#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include <QObject>

#include <dde-dock/dockplugininterface.h>
#include <libdui/dimagebutton.h>

#include "bluetoothobject.h"

class QSvgWidget;
class BluetoothPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-bluetooth-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    explicit BluetoothPlugin(QObject *parent = 0);
    ~BluetoothPlugin();

    QString getPluginName() Q_DECL_OVERRIDE;

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString id) Q_DECL_OVERRIDE;
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

    void removeItem(const QString &id);
    void addItem(const QString &id);
private:
    QString settingEnabledKey(const QString &id);

    DockPluginProxyInterface *m_proxy;
    BluetoothObject *m_bluetooth = nullptr;
    QMap<QString, QSvgWidget*> m_mapIdToButton;
    QSettings *m_settings;
};

#endif // BLUETOOTHPLUGIN_H
