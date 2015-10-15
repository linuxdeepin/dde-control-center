#include "diskitem.h"

DiskItem::DiskItem(const QString &id, DBusDiskMount *diskMount, QWidget *parent)
    : QLabel(parent), m_id(id)
{
    notifyInterface = new QDBusInterface("org.freedesktop.Notifications",
                                                         "/org/freedesktop/Notifications",
                                                         "org.freedesktop.Notifications",
                                                         QDBusConnection::sessionBus());
    connect(notifyInterface, SIGNAL(ActionInvoked(uint,QString)), SLOT(slotRetry(uint,QString)));

    m_diskMount = diskMount;
    connect(diskMount, &DBusDiskMount::DiskListChanged, this, &DiskItem::updateData);
    connect(diskMount, &DBusDiskMount::Error, this, &DiskItem::sendNotification);
    this->setFixedSize(220,80);
    initWidgets();

    updateData();
}

void DiskItem::updateData()
{
    DiskInfoList infoList = m_diskMount->diskList();
    foreach (DiskInfo info, infoList)
    {
        if (info.uUID == m_id)
        {
            m_diskUuid = info.uUID;

            m_titleLabel->setText(info.name);
            m_diskIcon->setIcon(info.icon);
            m_diskIcon->setMountPoint(info.mountPoint);
            m_progressLabel->setText(bitToHuman(info.used) + "/" + bitToHuman(info.size));
            m_usedBar->setValue(100 * (double(info.used) / info.size));
        }
    }
}

void DiskItem::sendNotification(const QString &title, const QString &msg)
{
    qDebug() << "send message:" << title << msg;

    notifyInterface->asyncCall("Notify",
                           "Disk Mount",
                           uint(0),
                           "diskmount",
                           title,
                           msg,
                           QStringList() << "diskmount_retry" << tr("Resend"),
                           QVariantMap(),
                           0);
}

void DiskItem::umountDisk()
{
    qWarning() << "Umount Disk: "<< m_diskUuid;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_diskMount->DeviceUnmount(m_diskUuid));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher]{
        if(!watcher->reply().arguments().first().toBool()){
             sendNotification("disk mount", "umount disk failed, plase retry");
        }

        watcher->deleteLater();
    });
}

void DiskItem::slotRetry(uint, QString id)
{
    if(id == "diskmount_retry"){
        umountDisk();
    }
}

void DiskItem::initWidgets()
{
    m_titleLabel = new QLabel("Disk",this);
    m_titleLabel->setObjectName("DiskTitle");
    m_titleLabel->setFixedSize(width(), 20);
    m_titleLabel->move(0,0);

    m_diskIcon = new DiskIcon(this);
    m_diskIcon->move(0,m_titleLabel->height() + 5);

    m_progressLabel = new QLabel("0GB/0GB", this);
    m_progressLabel->setFixedSize(width() - m_diskIcon->width() - 10, 20);
    m_progressLabel->move(m_diskIcon->x() + m_diskIcon->width() + 5, m_diskIcon->y());

    m_usedBar = new QProgressBar(this);
    m_usedBar->setRange(0,100);
    m_usedBar->setTextVisible(false);
    m_usedBar->setFixedSize(m_progressLabel->width(), 5);
    m_usedBar->move(m_progressLabel->x(),m_progressLabel->y() + m_progressLabel->height() + 5);


    UmountButton * uButton = new UmountButton(this);
    uButton->move(width() - uButton->width() - 5, m_usedBar->y() - uButton->height() - 5);
    connect(uButton, &UmountButton::mousePressed, this, &DiskItem::umountDisk);
}

QString DiskItem::bitToHuman(qint64 value)
{
    //default value is KB
    value *= 1000;
    qint64 kSize = 1000;
    qint64 mSize = 1000 * kSize;
    qint64 gSize = 1000 * mSize;

    if (value >= gSize)
        return QString::number(double(value) / gSize, 'f', 2) + "GB";
    else if (value >= mSize)
        return QString::number(double(value) / gSize, 'f', 2) + "MB";
    else if (value >= kSize)
        return QString::number(double(value) / gSize, 'f', 2) + "KB";
    else
        return QString::number(value) + "B";
}
