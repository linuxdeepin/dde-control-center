#include "appiconslider.h"

const int TITLE_LEFT_MARGIN = 30;
const int SLIDER_SPACING = 15;
const int SLIDER_HEIGHT = 30;
const int SLIDER_WIDTH = 140;
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
    if (!m_iLabel || !m_iSlider)
        return;

    if (qAbs(int(m_dasi->volume() * 100) - m_iSlider->value()) > 1)
        m_iSlider->setValue(m_dasi->volume() * 100);
    m_iLabel->setPixmap(SoundIcon::getAppSinkIcon(ICON_SIZE,m_dasi->icon()));
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
    connect(m_dasi, &DBusAudioSinkInput::MuteChanged, [=] {
        m_muteIcon->setVisible(m_dasi->mute());
        m_iSlider->setIsMute(m_dasi->mute());
    });
}

void AppIconSlider::initWidget()
{
    m_iLabel = new IconLabel(this);
    m_iLabel->setAlignment(Qt::AlignCenter);
    m_iLabel->setFixedSize(ICON_SIZE,ICON_SIZE);
    m_iLabel->setPixmap(SoundIcon::getAppSinkIcon(ICON_SIZE,m_dasi->icon()));
    connect(m_iLabel,&IconLabel::released,[=](){
        if (m_dasi->isValid())
            m_dasi->SetMute(!m_dasi->mute());
    });

    m_muteIcon = new QLabel(m_iLabel);
    m_muteIcon->setFixedSize(ICON_SIZE, ICON_SIZE);
    m_muteIcon->setPixmap(QPixmap(":/Resource/images/app-mute.png"));
    m_muteIcon->setVisible(m_dasi->mute());

    m_iSlider = new VolumeSlider(Qt::Horizontal, this);
    m_iSlider->setFixedSize(SLIDER_WIDTH, SLIDER_HEIGHT);
    m_iSlider->setMaximum(100);
    m_iSlider->setMinimum(0);
    m_iSlider->setIsMute(m_dasi->mute());
    connect(m_iSlider,&QSlider::valueChanged,[=] {
        if(!m_callDBusTimer) {
            m_callDBusTimer = new QTimer(this);
            m_callDBusTimer->setSingleShot(true);

            connect(m_callDBusTimer, &QTimer::timeout, m_iSlider, [this] {
                m_callDBusTimer->deleteLater();
                m_callDBusTimer = NULL;

                int value = m_iSlider->value();

                if (m_dasi->isValid() && qAbs(int(m_dasi->volume() * 100) - value) > 1) {
                    m_dasi->SetMute(false);
                    m_dasi->SetVolume(value / 100.00, m_dasi->mute());
                }
            });
        }

        m_callDBusTimer->start(300);
    });

    volumeUpdate();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(TITLE_LEFT_MARGIN, 0, 0, 0);
    mainLayout->setSpacing(SLIDER_SPACING);
    mainLayout->addWidget(m_iLabel, 0 , Qt::AlignLeft);
    mainLayout->addWidget(m_iSlider, 1, Qt::AlignLeft);

    this->setLayout(mainLayout);
    this->adjustSize();
}
