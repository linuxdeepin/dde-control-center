#ifndef DISKCONTENT_H
#define DISKCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QFile>
#include <QDebug>
#include "dbus/dbusdiskmount.h"
#include "dock/dockconstants.h"
#include "dock/dockpluginproxyinterface.h"
#include "diskitem.h"

class DiskContent : public QWidget
{
    Q_OBJECT
public:
    explicit DiskContent(const QString &uuid, DockPluginProxyInterface *proxy, QWidget *parent = 0);
    ~DiskContent();

public slots:

private:
    void initStyleSheet();
    void initDiskMount();

    void updateMountDisks();

private:
    QVBoxLayout * m_mainLayout = NULL;
    QMap<QString, DiskItem *> m_itemList;
    QString m_uuid = "";
    DockPluginProxyInterface * m_proxy = NULL;
    DBusDiskMount * m_diskMount = NULL;
};

#endif // DISKCONTENT_H
