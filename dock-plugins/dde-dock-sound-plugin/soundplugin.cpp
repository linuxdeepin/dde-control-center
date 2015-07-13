#include "soundplugin.h"


void SoundPlugin::init(DockPluginProxyInterface *proxier)
{
    m_proxier = proxier;


}

QStringList SoundPlugin::uuids()
{
    return m_items.keys();
}

QWidget * SoundPlugin::getItem(QString uuid)
{
    return NULL;
}

QWidget * SoundPlugin::getContents(QString uuid)
{
    return NULL;
}

void SoundPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{

}

QString SoundPlugin::name()
{
    return "Sound";
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-sound-plugin, SoundPlugin)
#endif // QT_VERSION < 0x050000
