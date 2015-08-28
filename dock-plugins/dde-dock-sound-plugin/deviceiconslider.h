#ifndef DEVICEICONSLIDER_H
#define DEVICEICONSLIDER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSlider>
#include <QDebug>
#include "iconlabel.h"
#include "soundicon.h"
#include "dbus/dbusaudiosink.h"
#include <dde-dock/dockconstants.h>

class DeviceIconSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceIconSlider(const QString &interfacePath, QWidget *parent = 0);

    void changeMode(Dock::DockMode mode);
signals:

public slots:
    void volumeUpdate();

private:
    void initSink(const QString &path);
    void initWidget();

private:
    Dock::DockMode m_dockMode = Dock::EfficientMode;
    QSlider *m_iSlider = NULL;
    IconLabel *m_iLabel = NULL;

    DBusAudioSink *m_das = NULL;
    const int ICON_SIZE = 24;
};

#endif // DEVICEICONSLIDER_H
