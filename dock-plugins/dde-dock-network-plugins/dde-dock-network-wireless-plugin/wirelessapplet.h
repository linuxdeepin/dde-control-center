#ifndef WIRELESSAPPLET_H
#define WIRELESSAPPLET_H

#include <QWidget>
#include <QLabel>
#include "wirelessappletitem.h"
#include "dbus/dbusnetwork.h"

#include <libdui/dboxwidget.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlistwidget.h>

DUI_USE_NAMESPACE

class WirelessApplet : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit WirelessApplet(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent = 0);
    ~WirelessApplet();

    int maxStrength();
    QString uuid() const;

signals:
    void sizeChanged();
    void propertiesChanged();
    void activeApChanged(QString ap);

private:
    void initStyleSheet();
    void initTitleLine();
    void initApListContent();
    void removeApFromList(const QString &apPath);
    void addApToList(const WirelessAppletItem::ApData &apData);
    bool removeOverlapApFromList(const WirelessAppletItem::ApData &apData);
    void onAccessPointAdded(const QString &devicePath, const QString &apInfo);
    void onAccessPointRemoved(const QString &devicePath, const QString &apInfo);
    void onApStrengthChanged(int strength);
    void onDevicesChanged();
    void onDeviceEnabledChanged(const QString &path, bool enable);

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    QLabel *m_titleLabel;
    DListWidget *m_listWidget;
    DSwitchButton *m_deviceSwitcher;
};

#endif // WIRELESSAPPLET_H
