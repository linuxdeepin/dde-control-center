#include "appiconslider.h"

AppIconSlider::AppIconSlider(const QString &interfacePath, QWidget *parent)
    : QWidget(parent),m_interfacePath(interfacePath)
{
    initSinkInput(interfacePath);
    initWidget();
}

QString AppIconSlider::interfacePath() const
{
    return m_interfacePath;
}

void AppIconSlider::volumeUpdate()
{
    if (m_dasi && m_iLabel && m_iSlider)
    {
        int volume = qRound(m_dasi->volume() * 100);
        m_iSlider->setValue(volume);

        m_iLabel->setPixmap(SoundIcon::getAppSinkIcon(ICON_SIZE,m_dasi->icon()));
    }
}

bool AppIconSlider::isValid()
{
    if (m_dasi && m_dasi->isValid())
        return true;
    else
        return false;
}

void AppIconSlider::initSinkInput(const QString &path)
{
    m_dasi = new DBusAudioSinkInput(path,this);
    connect(m_dasi,&DBusAudioSinkInput::VolumeChanged,this,&AppIconSlider::volumeUpdate);
}

void AppIconSlider::initWidget()
{
    m_iLabel = new IconLabel(this);
    m_iLabel->setAlignment(Qt::AlignCenter);
    m_iLabel->setFixedSize(ICON_SIZE,ICON_SIZE);
    connect(m_iLabel,&IconLabel::released,[=](){
       m_dasi->SetMute(!m_dasi->mute());
    });

    m_iSlider = new QSlider(Qt::Horizontal, this);
    m_iSlider->setFixedSize(155, 30);
    m_iSlider->setMaximum(100);
    m_iSlider->setMinimum(0);
    connect(m_iSlider,&QSlider::valueChanged,[=](int value){
        m_dasi->SetMute(false);
        m_dasi->SetVolume(value / 100.00, m_dasi->mute());
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
