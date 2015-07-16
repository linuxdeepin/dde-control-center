#include "soundcontent.h"

SoundContent::SoundContent(QWidget *parent) : QWidget(parent)
{
    initStyleSheet();
    initFrame();
}


void SoundContent::initStyleSheet()
{
    QFile file("://Resource/qss/default.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else
        qWarning() << "[Error:] Open  style file errr!";
}

void SoundContent::initFrame()
{
    DeviceFrame * deviceFrame = new DeviceFrame();
    deviceFrame->adjustSize();
    AppFrame * appFrame = new AppFrame();
    appFrame->adjustSize();
    connect(appFrame,&AppFrame::appCountChanged,[=](int count){
        qWarning() << "SinkInput Count:" << count;
        if (count <= 0)
        {
            appFrame->setVisible(false);
        }
        else
        {
            appFrame->setVisible(true);
            appFrame->adjustSize();
        }
    });

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(deviceFrame);
    mainLayout->addWidget(appFrame);
    mainLayout->setAlignment(deviceFrame,Qt::AlignHCenter);
    mainLayout->setAlignment(appFrame,Qt::AlignHCenter);

    setLayout(mainLayout);
    adjustSize();
}
