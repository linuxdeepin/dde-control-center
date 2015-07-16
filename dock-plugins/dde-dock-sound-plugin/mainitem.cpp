#include "mainitem.h"

MainItem::MainItem(QWidget *parent) : QLabel(parent)
{
    initDefaultSink();
}

void MainItem::setDockMode(Dock::DockMode mode)
{
    m_dockMode = mode;

    updateIcon();
}

void MainItem::wheelEvent(QWheelEvent * event)
{
    if (m_defaultSkin)
    {
        double tmp = m_defaultSkin->volume();
        if (event->angleDelta().y() > 0)
            m_defaultSkin->SetVolume(tmp > 0.9 ? 1.0 : tmp + WHEEL_STEP,m_defaultSkin->mute());
        else
            m_defaultSkin->SetVolume(tmp < 0.1 ? 0 : tmp - WHEEL_STEP,m_defaultSkin->mute());

        event->accept();

        updateIcon();
    }
}

void MainItem::initDefaultSink()
{
    DBusAudio *audio = new DBusAudio(this);
    QString path = QDBusObjectPath(audio->GetDefaultSink().value()).path();
    m_defaultSkin = new DBusAudioSink(path,this);
    connect(m_defaultSkin,&DBusAudioSink::VolumeChanged,this,&MainItem::updateIcon);
}

void MainItem::updateIcon()
{
    if (!m_defaultSkin)
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode,0,true));
    else
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode,int(m_defaultSkin->volume() * 100),m_defaultSkin->mute()));
}
