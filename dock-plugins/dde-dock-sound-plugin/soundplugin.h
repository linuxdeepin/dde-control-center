#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include <QMap>
#include <QUuid>
#include "dock/dockconstants.h"
#include "dock/dockplugininterface.h"
#include "dock/dockpluginproxyinterface.h"


class SoundPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-sound-plugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DockPluginInterface)


public:
    ~SoundPlugin();

    void init(DockPluginProxyInterface * proxier) Q_DECL_OVERRIDE;

    QStringList uuids() Q_DECL_OVERRIDE;
    QWidget * getItem(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getContents(QString uuid) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode);

    QString name() Q_DECL_OVERRIDE;

private:
    QMap<QString, QWidget*> m_items;
    DockPluginProxyInterface * m_proxier = NULL;
    Dock::DockMode m_mode;

};

#endif // SOUNDPLUGIN_H
