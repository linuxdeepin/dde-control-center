#ifndef SOUNDICON_H
#define SOUNDICON_H

#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include "dde-dock/dockconstants.h"

class SoundIcon : public QObject
{
    Q_OBJECT
public:
    SoundIcon();

    static QPixmap getDefaultSinkDockIcon(Dock::DockMode mode, int volume, bool isMute = false);
    static QPixmap getDefaultSinkIcon(Dock::DockMode mode, int volume, bool isMute = false);
    static QPixmap getDefaultSinkIcon(int size, int volume, bool isMute = false);
    static QPixmap getAppSinkIcon(Dock::DockMode mode,const QString &iconName);
    static QPixmap getAppSinkIcon(int size, const QString &iconName);

};

#endif // SOUNDICON_H
