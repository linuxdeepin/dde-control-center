#include "appframe.h"

AppFrame::AppFrame(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(10);

    setLayout(m_mainLayout);
    initTitle();
    initAppList();
    adjustSize();
}

void AppFrame::initTitle()
{
    QLabel *titleLabel = new QLabel("Application");
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

void AppFrame::initAppList()
{
    m_audio = new DBusAudio(this);
    QList<QDBusObjectPath> pathList = m_audio->sinkInputs();

    if (pathList.count() <= 0)
    {
        setVisible(false);
    }

    for (int i = 0; i < pathList.length(); i ++)
    {
        QString path = pathList.at(i).path();
        AppIconSlider *iconSlider = new AppIconSlider(path,this);
        if (iconSlider->isValid())
        {
            iconSlider->setFixedSize(240,30);
            m_mainLayout->addWidget(iconSlider);
            m_mainLayout->setAlignment(iconSlider,Qt::AlignRight);

            if (m_appsMap.keys().indexOf(path) == -1)
                m_appsMap.insert(path,iconSlider);
        }
        else
        {
            delete iconSlider;
            break;
        }

    }
    connect(m_audio,&DBusAudio::SinkInputsChanged,this,&AppFrame::updateAppList);
}

void AppFrame::updateAppList()
{
    QList<QDBusObjectPath> pathList = m_audio->sinkInputs();

    int tmpCount = pathList.count();

    QStringList tmplist;
    foreach (QDBusObjectPath tmpPath, pathList) {
        if (tmplist.indexOf(tmpPath.path()) == -1)
            tmplist.append(tmpPath.path());
    }

    for (int i = 0; i < tmplist.length(); i ++)
    {
        QString tmpPath = tmplist.at(i);
        if (m_appsMap.keys().indexOf(tmplist.at(i)) == -1)//new path,add to list
        {
            AppIconSlider *iconSlider = new AppIconSlider(tmpPath,this);
            if (iconSlider->isValid())
            {
                iconSlider->setFixedSize(240,30);
                m_mainLayout->addWidget(iconSlider);
                m_mainLayout->setAlignment(iconSlider,Qt::AlignRight);
                m_appsMap.insert(tmpPath,iconSlider);
            }
            else
                delete iconSlider;

            return;
        }
    }

    for (int i = 0; i < m_appsMap.count(); i ++)
    {
        if (tmplist.indexOf(m_appsMap.keys().at(i)) == -1)//path has remove
        {
            m_mainLayout->removeWidget(m_appsMap.value(m_appsMap.keys().at(i)));
            AppIconSlider *tmpSlider = m_appsMap.take(m_appsMap.keys().at(i));
            if (tmpSlider)
                delete tmpSlider;
            break;
        }
    }

    emit appCountChanged(tmpCount);
}
