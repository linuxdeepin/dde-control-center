#include "deviceiconslider.h"

DeviceIconSlider::DeviceIconSlider(const QString &interfacePath, QWidget *parent) : QWidget(parent)
{
    initSink(interfacePath);
    initWidget();
}

void DeviceIconSlider::changeMode(Dock::DockMode mode)
{
    m_dockMode = mode;
}

void DeviceIconSlider::volumeUpdate()
{
    if (m_das && m_iLabel && m_iSlider)
    {
        int volume = qRound(m_das->volume() * 100);
        m_iSlider->setValue(volume);
        m_iLabel->setPixmap(SoundIcon::getDefaultSinkIcon(ICON_SIZE,volume,m_das->mute()));
    }
}

void DeviceIconSlider::initSink(const QString &path)
{
    m_das = new DBusAudioSink(path,this);
    connect(m_das,&DBusAudioSink::VolumeChanged,this,&DeviceIconSlider::volumeUpdate);
    connect(m_das, &DBusAudioSink::MuteChanged, this, &DeviceIconSlider::volumeUpdate);
}

void DeviceIconSlider::initWidget()
{
    m_iLabel = new IconLabel(this);
    m_iLabel->setAlignment(Qt::AlignCenter);
    m_iLabel->setFixedSize(ICON_SIZE,ICON_SIZE);
    connect(m_iLabel,&IconLabel::released,[=](){
       m_das->SetMute(!m_das->mute());
    });

    m_iSlider = new QSlider(Qt::Horizontal, this);
    m_iSlider->setFixedSize(155,30);
    m_iSlider->setMaximum(100);
    m_iSlider->setMinimum(0);
    connect(m_iSlider,&QSlider::valueChanged,[=](int value){
        m_das->SetMute(false);
        m_das->SetVolume(value / 100.00, m_das->mute());
    });

    volumeUpdate();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(m_iLabel);
    mainLayout->addWidget(m_iSlider);
    mainLayout->setAlignment(m_iLabel,Qt::AlignRight);
    mainLayout->setAlignment(m_iSlider,Qt::AlignRight);

    this->setLayout(mainLayout);
    this->adjustSize();
}
