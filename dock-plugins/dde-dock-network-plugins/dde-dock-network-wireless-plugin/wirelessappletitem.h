#ifndef WIRELESSAPPLETITEM_H
#define WIRELESSAPPLETITEM_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "dbus/dbusnetwork.h"
#include "libdui/dloadingindicator.h"
#include "libdui/dimagebutton.h"
#include "../network-data/networkdata.h"

DUI_USE_NAMESPACE
using namespace NetworkPlugin;

class WirelessAppletItem : public QFrame
{
    Q_OBJECT
public:    
    struct ApData {
        QString apPath;
        QString ssid;
        int strength = -1;
        bool secured = false;
        bool securedInEap = true;
    };

    explicit WirelessAppletItem(const ApData &data, const QString &devicePath, DBusNetwork *dbusNetwork, QWidget *parent = 0);

    QString getApPath() const;
    ApData getApData() const;

public slots:
    void onActiveApChanged();
    void onActiveConnectionsChanged();

signals:
    void strengthChanged(int strength);

private:
    QPixmap getPixmapByStrength();
    void updateConnectionState();
    void onAccessPointPropertiesChanged(const QString &devicePath, const QString &info);

private:
    DBusNetwork *m_dbusNetwork;
    QString m_devicePath;
    ApData m_apData;

    QPushButton *m_title;
    DImageButton *m_checkIcon;
    DLoadingIndicator *m_loadingIcon;
    QLabel *m_strengthIcon;
};

#endif // WIRELESSAPPLETITEM_H
