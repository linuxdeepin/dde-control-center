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
#include <libdui/dinputdialog.h>

DUI_USE_NAMESPACE

class WirelessApplet : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit WirelessApplet(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent = 0);
    ~WirelessApplet();

    int maxStrength();

signals:
    void sizeChanged();
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
    void onNeedSecrets(const QString &in0, const QString &in1, const QString &in2, bool in3);

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    QLabel *m_titleLabel;
    DListWidget *m_listWidget;
    DSwitchButton *m_deviceSwitcher;
    DInputDialog *m_passworkInputDialog = nullptr;
    QString m_targetDevicePath;
    QString m_tragetConnectUuid;
};

#endif // WIRELESSAPPLET_H
