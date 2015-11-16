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
    void addApToList(const WirelessAppletItem::ApData &apData);
    void removeApFromList(const QString &apPath);
    void onAccessPointAdded(const QString &devicePath, const QString &apInfo);
    void onAccessPointRemoved(const QString &devicePath, const QString &apInfo);
    void onApStrengthChanged(int strength);
    void onDevicesChanged();

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    QLabel *m_titleLabel;
    DListWidget *m_listWidget;
    DSwitchButton *m_deviceSwitcher;

    const int FRAME_WIDTH = 220;
    const int TITLE_HEIGHT = 36;
    const int LINE_HEIGHT = 24;
    const int TITLE_LEFT_MARGIN = 20;
    const int TITLE_RIGHT_MARGIN = 10;
};

#endif // WIRELESSAPPLET_H
