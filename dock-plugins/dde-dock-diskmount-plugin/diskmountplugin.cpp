#include "diskmountplugin.h"


DiskMountPlugin::DiskMountPlugin()
{
    m_diskMount = new DBusDiskMount(this);
    connect(m_diskMount,&DBusDiskMount::DiskListChanged,this,&DiskMountPlugin::mountableDeviceChanged);
}


void DiskMountPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    mountableDeviceChanged();
    setMode(proxy->dockMode());
}

QString DiskMountPlugin::getPluginName()
{
    return "DiskMount plugin";
}

QStringList DiskMountPlugin::ids()
{
    return QStringList(m_id);
}

QString DiskMountPlugin::getName(QString)
{
    return getPluginName();
}

QString DiskMountPlugin::getTitle(QString)
{
    return getPluginName();
}

QString DiskMountPlugin::getCommand(QString)
{
    return "";
}

bool DiskMountPlugin::canDisable(QString)
{
    return false;
}

bool DiskMountPlugin::isDisabled(QString)
{
    return false;
}

void DiskMountPlugin::setDisabled(QString, bool)
{

}

QWidget * DiskMountPlugin::getItem(QString)
{
    MainItem * item = new MainItem();
    connect(this,&DiskMountPlugin::dockModeChanged,item,&MainItem::setDockMode);
    item->setDockMode(m_mode);

    m_itemList.append(item);

    return item;
}

QWidget * DiskMountPlugin::getApplet(QString)
{
    if (!m_diskContent)
        m_diskContent = new DiskContent(m_id, m_proxy);
    return m_diskContent;
}

void DiskMountPlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
}

QString DiskMountPlugin::getMenuContent(QString)
{
//    QJsonObject contentObj;

//    QJsonArray items;

//    items.append(createMenuItem("undock", "Undock"));

//    contentObj.insert("items", items);

//    return QString(QJsonDocument(contentObj).toJson());

    return "";
}

void DiskMountPlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    qWarning() << "Menu check:" << itemId << checked;
}

void DiskMountPlugin::mountableDeviceChanged()
{
    int mountableCount = 0;
    DiskInfoList tmpList = m_diskMount->diskList();
    foreach (DiskInfo info, tmpList) {
        if (info.canUnmount)
        {
            mountableCount ++;
            break;
        }
    }

    if (mountableCount > 0)
    {
        if (m_itemList.count() == 0)
            m_proxy->itemAddedEvent(m_id);
    }
    else if (m_itemList.count() > 0)
    {
        m_itemList.clear();
        m_proxy->itemRemovedEvent(m_id);
    }
}

// private methods
void DiskMountPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    emit dockModeChanged(mode);

    m_proxy->itemSizeChangedEvent(m_id);
}

QJsonObject DiskMountPlugin::createMenuItem(QString itemId, QString itemName, bool checkable, bool checked)
{
    QJsonObject itemObj;

    itemObj.insert("itemId", itemId);
    itemObj.insert("itemText", itemName);
    itemObj.insert("itemIcon", "");
    itemObj.insert("itemIconHover", "");
    itemObj.insert("itemIconInactive", "");
    itemObj.insert("itemExtra", "");
    itemObj.insert("isActive", true);
    itemObj.insert("isCheckable", checkable);
    itemObj.insert("checked", checked);
    itemObj.insert("itemSubMenu", QJsonObject());

    return itemObj;
}


DiskMountPlugin::~DiskMountPlugin()
{
    if (m_diskContent)
        m_diskContent->deleteLater();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-diskmount-plugin, DiskMountPlugin)
#endif // QT_VERSION < 0x050000
