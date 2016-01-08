#include "deviceiconslider.h"

const int TITLE_LEFT_MARGIN = 30;
const int SLIDER_SPACING = 15;
const int SLIDER_HEIGHT = 30;
const int SLIDER_WIDTH = 140;
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
    if (!m_iLabel || !m_iSlider)
        return;

    int volume = m_das->volume() * 100;
    if (qAbs(int(m_das->volume() * 100) - m_iSlider->value()) > 1)
        m_iSlider->setValue(volume);
    m_iLabel->setPixmap(SoundIcon::getDefaultSinkIcon(ICON_SIZE, volume, m_das->mute()));
    m_iSlider->setIsMute(m_das->mute());
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
    m_iLabel->setPixmap(SoundIcon::getDefaultSinkIcon(ICON_SIZE, 0, true));
    connect(m_iLabel,&IconLabel::released,[=](){
        if (m_das->isValid())
            m_das->SetMute(!m_das->mute());
    });

    m_iSlider = new VolumeSlider(Qt::Horizontal, this);
    m_iSlider->setFixedSize(SLIDER_WIDTH, SLIDER_HEIGHT);
    m_iSlider->setMaximum(100);
    m_iSlider->setMinimum(0);
    m_iSlider->setIsMute(m_das->mute());
    connect(m_iSlider,&QSlider::valueChanged,[=] {
        if(!m_callDBusTimer) {
            m_callDBusTimer = new QTimer(this);
            m_callDBusTimer->setSingleShot(true);

            connect(m_callDBusTimer, &QTimer::timeout, m_iSlider, [this] {
                m_callDBusTimer->deleteLater();
                m_callDBusTimer = NULL;

                int value = m_iSlider->value();

                if (m_das->isValid() && qAbs(int(m_das->volume() * 100) - value) > 1) {
                    m_das->SetMute(false);
                    m_das->SetVolume(value / 100.00, m_das->mute());
                }
            });
        }

        m_callDBusTimer->start(300);
    });

    volumeUpdate();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(TITLE_LEFT_MARGIN, 0, 0, 0);
    mainLayout->setSpacing(SLIDER_SPACING);
    mainLayout->addWidget(m_iLabel, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_iSlider, 1, Qt::AlignLeft);

    this->setLayout(mainLayout);
    this->adjustSize();
}
