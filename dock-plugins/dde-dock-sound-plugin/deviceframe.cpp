#include "deviceframe.h"

const int TITLE_WIDTH = 240;
const int SPLIT_WIDTH = 150;
const int TITLE_LEFT_MARGIN = 20;
const int SLIDER_HEIGHT = 30;
DeviceFrame::DeviceFrame(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);
    initTitle();
    initDevice();
    adjustSize();
}


void DeviceFrame::initTitle()
{
    QLabel *titleLabel = new QLabel(tr("Device"));
    QLabel *splitLabel = new QLabel();
    splitLabel->setObjectName("SplitLine");
    splitLabel->setFixedSize(SPLIT_WIDTH, 1);
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(TITLE_LEFT_MARGIN, 0, 0, 0);
    titleLayout->setSpacing(10);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignLeft);
    titleLayout->addWidget(splitLabel, 1, Qt::AlignLeft);
    QWidget *titleWidget = new QWidget();
    titleWidget->setLayout(titleLayout);
    titleWidget->setFixedSize(TITLE_WIDTH, 35);

    m_mainLayout->addWidget(titleWidget);
    m_mainLayout->setAlignment(titleWidget,Qt::AlignRight);
}

void DeviceFrame::initDevice()
{
    DBusAudio *audio = new DBusAudio(this);
    QString path = QDBusObjectPath(audio->GetDefaultSink().value()).path();
    DeviceIconSlider *iconSlider = new DeviceIconSlider(path,this);
    iconSlider->setFixedSize(TITLE_WIDTH, SLIDER_HEIGHT);

    m_mainLayout->addWidget(iconSlider);
    m_mainLayout->setAlignment(iconSlider, Qt::AlignRight);

}
