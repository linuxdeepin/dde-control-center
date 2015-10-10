#ifndef DEFAULTAPPS_H
#define DEFAULTAPPS_H

#include <QObject>
#include <QtPlugin>
#include <QButtonGroup>
#include <QThread>

#include "moduleheader.h"

#include "interfaces.h"
#include "dbus/dbusdefaultapps.h"
#include "dbus/dbusdefaultmedia.h"

#include <libdui/darrowlineexpand.h>
#include <libdui/dexpandgroup.h>
#include <libdui/dswitchbutton.h>

DUI_USE_NAMESPACE

class QLabel;
class QFrame;
class DefaultApps : public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "default_applications.json")
    Q_INTERFACES(ModuleInterface)

public:
    DefaultApps();
    ~DefaultApps() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

public slots:
    void reset();

private:
    enum DefaultAppsCategory {
        Browser, Mail, Text, Music, Video, Picture, Terminal,
        CD_Audio, DVD_Video, MusicPlayer, Camera, Software,
    };

private:
    DArrowLineExpand *createDefaultAppsExpand(const DefaultAppsCategory & category, DArrowLineExpand *defaultApps);
    const QString getTypeByCategory(const DefaultAppsCategory & category);
    const QStringList getTypeListByCategory(const DefaultAppsCategory & category);
    void setMediaOptionVisible(const bool visible);

private slots:
    void lazyLoad();

private:
    QFrame * m_centralWidget;

    ModuleHeader *m_header;

    DBusDefaultApps m_dbusDefaultApps;
    DBusDefaultMedia m_dbusDefaultMedia;

    DExpandGroup *m_appGrp;
    DExpandGroup *m_mediaGrp;

    DArrowLineExpand *m_modBrowser;
    DArrowLineExpand *m_modMail;
    DArrowLineExpand *m_modText;
    DArrowLineExpand *m_modMusic;
    DArrowLineExpand *m_modVideo;
    DArrowLineExpand *m_modPicture;
    DArrowLineExpand *m_modTerminal;

    DArrowLineExpand *m_modCDAudio;
    DArrowLineExpand *m_modDVDVideo;
    DArrowLineExpand *m_modMusicPlayer;
    DArrowLineExpand *m_modCamera;
    DArrowLineExpand *m_modSoftware;

    DSwitchButton *m_autoPlaySwitch;
};

class SetDefAppsThread : public QThread
{
    Q_OBJECT

public:
    SetDefAppsThread(DBusDefaultApps *dbus, const QString &mime, const QString &appName, const QStringList &list);
    void run();

private:
    DBusDefaultApps *dbus;
    QString mime;
    QString appName;
    QStringList list;
};

class SetDefMediaThread : public QThread
{
    Q_OBJECT

public:
    SetDefMediaThread(DBusDefaultMedia *dbus, const QString &mime, const QString &appName, const QStringList &list);
    void run();

private:
    DBusDefaultMedia *dbus;
    QString mime;
    QString appName;
    QStringList list;
};

#endif
