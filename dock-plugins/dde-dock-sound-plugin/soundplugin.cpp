#include "soundplugin.h"

SoundPlugin::SoundPlugin()
{
    m_item = new MainItem();
}

void SoundPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_proxy->itemAddedEvent(m_uuid);

    setMode(proxy->dockMode());
}

QString SoundPlugin::name()
{
    return "Sound plugin";
}

QStringList SoundPlugin::uuids()
{
    return QStringList(m_uuid);
}

QString SoundPlugin::getTitle(QString)
{
    return name();
}

QWidget * SoundPlugin::getItem(QString)
{
    return m_item;
}

QWidget * SoundPlugin::getApplet(QString)
{
    return new SoundContent(m_uuid,m_proxy);
}

void SoundPlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
}

QString SoundPlugin::getMenuContent(QString)
{
    QJsonObject contentObj;

    QJsonArray items;

    items.append(createMenuItem("sound_setting", "Sound settings"));

    contentObj.insert("items", items);

    return QString(QJsonDocument(contentObj).toJson());
}

void SoundPlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    qWarning() << "Menu check:" << itemId << checked;
}

// private methods
void SoundPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    switch (mode) {
    case Dock::FashionMode:
        m_item->resize(Dock::APPLET_FASHION_ITEM_WIDTH,Dock::APPLET_FASHION_ITEM_HEIGHT);
        break;
    case Dock::EfficientMode:
        m_item->resize(Dock::APPLET_EFFICIENT_ITEM_WIDTH,Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
        break;
    case Dock::ClassicMode:
        m_item->resize(Dock::APPLET_CLASSIC_ITEM_WIDTH,Dock::APPLET_CLASSIC_ITEM_HEIGHT);
        break;
    default:
        break;
    }

    m_item->setDockMode(mode);
    m_proxy->itemSizeChangedEvent(m_uuid);
}

QJsonObject SoundPlugin::createMenuItem(QString itemId, QString itemName, bool checkable, bool checked)
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

SoundPlugin::~SoundPlugin()
{

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-sound-plugin, SoundPlugin)
#endif // QT_VERSION < 0x050000
