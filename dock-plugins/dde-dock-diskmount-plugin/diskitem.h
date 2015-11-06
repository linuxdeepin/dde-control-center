#ifndef DISKITEM_H
#define DISKITEM_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QDebug>
#include "libdui/dimagebutton.h"
#include "dbus/dbusdiskmount.h"
#include "diskicon.h"

DUI_USE_NAMESPACE

class DiskItem : public QLabel
{
    Q_OBJECT
public:
    explicit DiskItem(const QString &id,DBusDiskMount * diskMount , QWidget *parent = 0);

public slots:
    void updateData();

private:
    void initWidgets();
    QString bitToHuman(qint64 value);
    void sendNotification(const QString &title, const QString &msg);

private slots:
    void umountDisk();
    void slotRetry(uint, QString);
    void updateUnMountButtonState(bool normal);
    void umountDiskFailed(const QString &uuid, const QString &reason);

private:
    DBusDiskMount * m_diskMount = NULL;
    QString m_id = "";
    QString m_diskUuid = "";
    QString m_diskType;
    QLabel * m_titleLabel = NULL;
    DiskIcon * m_diskIcon = NULL;
    QLabel * m_progressLabel = NULL;
    QProgressBar * m_usedBar = NULL;
    QDBusInterface *notifyInterface;
    DImageButton *m_unMountButton;
};

#endif // DISKITEM_H
