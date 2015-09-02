#include <QFile>

#include "soundplugin.h"

SoundPlugin::SoundPlugin()
{
    m_item = new MainItem();

    this->initSettings();
}

void SoundPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    setMode(proxy->dockMode());

    onDisableChanged();
}

QString SoundPlugin::getPluginName()
{
    return tr("Sound");
}

QStringList SoundPlugin::ids()
{
    return QStringList(m_id);
}

QString SoundPlugin::getName(QString)
{
    return getPluginName();
}

QString SoundPlugin::getTitle(QString)
{
    return getPluginName();
}

QString SoundPlugin::getCommand(QString)
{
    return "dde-control-center sound";
}

QPixmap SoundPlugin::getIcon(QString id)
{
     return SoundIcon::getDefaultSinkDockIcon(Dock::EfficientMode, 100);
}

bool SoundPlugin::canDisable(QString)
{
    return true;
}

bool SoundPlugin::isDisabled(QString)
{
    return m_settings->value(settingDisabledKey()).toBool();
}

void SoundPlugin::setDisabled(QString id, bool disabled)
{
    if (id != m_id)
        return;

    m_settings->setValue(settingDisabledKey(), disabled);

    onDisableChanged();
}

QWidget * SoundPlugin::getItem(QString)
{
    bool disabled = m_settings->value(settingDisabledKey()).toBool();

    return disabled ? NULL : m_item;
}

QWidget * SoundPlugin::getApplet(QString)
{
    if (!m_soundContent)
        m_soundContent = new SoundContent(m_id, m_proxy);
    return m_soundContent;
}

void SoundPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (newMode != oldMode){
        setMode(newMode);
        onDisableChanged();
    }
}

QString SoundPlugin::getMenuContent(QString)
{
    return "";
}

void SoundPlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    qWarning() << "Menu check:" << itemId << checked;
}

// private methods
void SoundPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-sound-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        m_settings->setValue(settingDisabledKey(), true);
    }
}

void SoundPlugin::onDisableChanged()
{
    m_proxy->itemRemovedEvent(m_id);
    m_item->setParent(NULL);

    if (!isDisabled(m_id))
        m_proxy->itemAddedEvent(m_id);
}

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
    m_proxy->itemSizeChangedEvent(m_id);
}

QString SoundPlugin::settingDisabledKey()
{
    return QString("%1/disabled").arg(m_mode);
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
    if (m_soundContent)
        m_soundContent->deleteLater();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-sound-plugin, SoundPlugin)
#endif // QT_VERSION < 0x050000
