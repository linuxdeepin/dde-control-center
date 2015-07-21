#ifndef WIREDCOMPONENT_H
#define WIREDCOMPONENT_H

#include <QObject>

#include "dbus/dbusnetwork.h"

#include "plugincomponentinterface.h"

class QLabel;
class WiredComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    WiredComponent(QObject *parent = 0);

    QString getUUID() Q_DECL_OVERRIDE;

    QString getTitle() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QLabel * m_item;
    com::deepin::daemon::Network * m_dbusNetwork;

    QString getWiredIP();

    bool hasWiredDevices();
    bool hasWirelessDevices();
    bool isAnyWiredActive();
    bool isAnyWirelessActive();
    bool shouldShowItem();

    void updateItem();
};

#endif // WIREDCOMPONENT_H
