#include "diskcontent.h"

DiskContent::DiskContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent)
    : QWidget(parent), m_id(id), m_proxy(proxy)
{
    initStyleSheet();
    initDiskMount();

    m_mainLayout = new QVBoxLayout(this);
    setLayout(m_mainLayout);

    updateMountDisks();
}

void DiskContent::initStyleSheet()
{
    QFile file("://Resource/qss/disk.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);

        file.close();
    }
    else
        qWarning() << "[Error:] Open  style file errr!";
}

void DiskContent::initDiskMount()
{
    m_diskMount = new DBusDiskMount(this);
    connect(m_diskMount,&DBusDiskMount::DiskListChanged,this,&DiskContent::updateMountDisks);
}

void DiskContent::updateMountDisks()
{
    DiskInfoList tmpList = m_diskMount->diskList();
    QStringList idList;
    foreach (DiskInfo info, tmpList)
    {
        if (info.canUnmount)
            idList << info.uUID;
    }

    foreach (DiskInfo info, tmpList)
    {
        if (info.canUnmount && m_itemList.keys().indexOf(info.uUID) == -1)//New Item
        {
            DiskItem * item = new DiskItem(info.uUID,m_diskMount, this);
            m_itemList.insert(info.uUID,item);
            m_mainLayout->addWidget(item);

            qWarning() << "Disk Mounted:" << info.uUID;
        }
    }

    foreach (QString id, m_itemList.keys())
    {
        if (idList.indexOf(id) == -1)//Not in can-mount list
        {
            DiskItem * item = m_itemList.take(id);
            m_mainLayout->removeWidget(item);
            item->deleteLater();

            qWarning() << "Disk Unmounted:" << id;
        }
    }

    adjustSize();

    m_proxy->infoChangedEvent(DockPluginInterface::AppletSize, m_id);
}

DiskContent::~DiskContent()
{

}
