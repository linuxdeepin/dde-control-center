#ifndef DEFAULTAPPS_H
#define DEFAULTAPPS_H

#include <QObject>
#include <QtPlugin>
#include <QButtonGroup>

#include "interfaces.h"
#include "dbus/dbusdefaultapps.h"

#include <libdui/darrowlineexpand.h>

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
    void Test();

private:
    enum DefaultAppsCategory {
        Browser, Mail, Text, Music, Video, Picture, Terminal, CD_Audio, DVD_Video, MusicPlayer, Camera, Software,
    };

private:
    DUI::DArrowLineExpand *createDefaultAppsExpand(const DefaultAppsCategory & category);
    AppList getAppsListByCategory(const DefaultAppsCategory & category);

private:
    QFrame * m_centralWidget;

    DefaultDBusApps m_dbusDefaultApps;
};

#endif
