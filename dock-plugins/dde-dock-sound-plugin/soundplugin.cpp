#include <QFile>

#include "soundplugin.h"

static const QString SettingsEnabledKey = "SoundPlugin/enabled";

SoundPlugin::SoundPlugin()
{
    m_item = new MainItem();

    this->initSettings();
}

void SoundPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_proxy->itemAddedEvent(m_id);

    setMode(proxy->dockMode());
}

QString SoundPlugin::getPluginName()
{
    return "Sound plugin";
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

bool SoundPlugin::canDisable(QString id)
{
    return true;
}

bool SoundPlugin::isDisabled(QString)
{
    return m_settings->value(SettingsEnabledKey).toBool();
}

void SoundPlugin::setDisabled(QString id, bool disabled)
{
    if (disabled) {
        m_item->setParent(NULL);
        m_proxy->itemRemovedEvent(id);
    }

    m_settings->setValue(SettingsEnabledKey, !disabled);
}

QWidget * SoundPlugin::getItem(QString)
{
    bool enabled = m_settings->value(SettingsEnabledKey).toBool();

    return enabled ? m_item : NULL;
}

QWidget * SoundPlugin::getApplet(QString)
{
    if (!m_soundContent)
        m_soundContent = new SoundContent(m_id, m_proxy);
    return m_soundContent;
}

void SoundPlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
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
        m_settings->setValue(SettingsEnabledKey, true);
    }
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
