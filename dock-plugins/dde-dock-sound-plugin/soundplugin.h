#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include <QMap>
#include <QLabel>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QDebug>
#include "dock/dockconstants.h"
#include "dock/dockplugininterface.h"
#include "dock/dockpluginproxyinterface.h"
#include "mainitem.h"
#include "soundcontent.h"


class SoundPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-sound-plugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DockPluginInterface)


public:
    SoundPlugin();
    ~SoundPlugin();

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString name() Q_DECL_OVERRIDE;

    QStringList uuids() Q_DECL_OVERRIDE;
    QString getTitle(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getItem(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString uuid) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString uuid) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString uuid, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_uuid = "sound_plugin";
    DockPluginProxyInterface * m_proxy;

    Dock::DockMode m_mode;
    MainItem * m_item = NULL;

private:
    void setMode(Dock::DockMode mode);
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);

};

#endif // SOUNDPLUGIN_H
