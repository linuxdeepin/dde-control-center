#ifndef MAINITEM_H
#define MAINITEM_H

#include <QLabel>
#include <QWheelEvent>
#include <QDebug>
#include "dbus/dbusaudio.h"
#include "dbus/dbusaudiosink.h"
#include "dock/dockconstants.h"
#include "soundicon.h"

class MainItem : public QLabel
{
    Q_OBJECT
public:
    MainItem(QWidget *parent = 0);
    void setDockMode(Dock::DockMode mode);

protected:
    void wheelEvent(QWheelEvent *);

private:
    void initDefaultSink();
    void updateIcon();

private:
    DBusAudioSink *m_defaultSkin = NULL;
    Dock::DockMode m_dockMode = Dock::FashionMode;
    const double WHEEL_STEP = 0.1;
};

#endif // MAINITEM_H
