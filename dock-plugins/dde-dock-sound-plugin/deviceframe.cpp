#include "deviceframe.h"

DeviceFrame::DeviceFrame(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(10);

    setLayout(m_mainLayout);
    initTitle();
    initDevice();
    adjustSize();
}


void DeviceFrame::initTitle()
{
    QLabel *titleLabel = new QLabel("Device");
    QLabel *splitLabel = new QLabel();
    splitLabel->setObjectName("SplitLine");
    splitLabel->setFixedSize(155,1);
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setMargin(0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(splitLabel);
    titleLayout->setAlignment(titleLabel,Qt::AlignRight);
    titleLayout->setAlignment(splitLabel,Qt::AlignRight);
    QWidget *titleWidget = new QWidget();
    titleWidget->setLayout(titleLayout);
    titleWidget->setFixedSize(240,35);

    m_mainLayout->addWidget(titleWidget);
    m_mainLayout->setAlignment(titleWidget,Qt::AlignRight);
}

void DeviceFrame::initDevice()
{
    DBusAudio *audio = new DBusAudio(this);
    QString path = QDBusObjectPath(audio->GetDefaultSink().value()).path();
    DeviceIconSlider *iconSlider = new DeviceIconSlider(path,this);
    iconSlider->setFixedSize(240,30);

    m_mainLayout->addWidget(iconSlider);
    m_mainLayout->setAlignment(iconSlider,Qt::AlignRight);

}
