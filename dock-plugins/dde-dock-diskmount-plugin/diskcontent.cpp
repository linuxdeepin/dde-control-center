#include "diskcontent.h"

const int DISK_ITEM_HEIGHT = 80;
const int DISK_ITEM_WIDTH = 220;
const int DISK_ITEM_MARGIN = 10;
const int DISK_ITEM_SPACING = 6;
DiskContent::DiskContent(const QString &id, DockPluginProxyInterface *proxy, QWidget *parent)
    : QWidget(parent), m_id(id), m_proxy(proxy)
{
    initStyleSheet();
    initDiskMount();

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(DISK_ITEM_MARGIN, DISK_ITEM_MARGIN, DISK_ITEM_MARGIN, DISK_ITEM_MARGIN);
    m_mainLayout->setSpacing(DISK_ITEM_SPACING);
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
    bool infoChanged = false;
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

            infoChanged = true;
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

            infoChanged = true;
            qWarning() << "Disk Unmounted:" << id;
        }
    }

    int spacing = DISK_ITEM_MARGIN + DISK_ITEM_SPACING;
    setFixedSize(DISK_ITEM_WIDTH + DISK_ITEM_MARGIN * 2,
                 (DISK_ITEM_HEIGHT + spacing) * m_mainLayout->count() - spacing);

    if (infoChanged)
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, m_id);
}

DiskContent::~DiskContent()
{

}
