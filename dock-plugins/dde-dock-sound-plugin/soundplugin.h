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
#include <QSettings>
#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>
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

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString id) Q_DECL_OVERRIDE;
    bool canDisable(QString) Q_DECL_OVERRIDE;
    bool isDisabled(QString id) Q_DECL_OVERRIDE;
    void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_id = "sound_plugin";
    DockPluginProxyInterface * m_proxy;

    Dock::DockMode m_mode = Dock::FashionMode;
    MainItem * m_item = NULL;
    SoundContent *m_soundContent = NULL;
    QSettings * m_settings = NULL;

    void initSettings();
    void onDisableChanged();
    void setMode(Dock::DockMode mode);
    QString settingDisabledKey();
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);

};

#endif // SOUNDPLUGIN_H
