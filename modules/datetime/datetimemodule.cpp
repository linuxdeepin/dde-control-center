#include "datetimemodule.h"

DatetimeModule::DatetimeModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_work(new DatetimeWork(this)),
      m_model(new DatetimeModel(this)),
      m_datetimeWidget(nullptr),
      m_dateSettings(nullptr)
{
    connect(m_work, SIGNAL(NTPChanged(bool)), m_model, SLOT(setNTP(bool)));
    m_model->setNTP(m_work->nTP());
}

void DatetimeModule::initialize()
{

}

void DatetimeModule::moduleActive()
{

}

void DatetimeModule::moduleDeactive()
{
    if(m_datetimeWidget)
    {
        m_datetimeWidget->deleteLater();
        m_datetimeWidget = nullptr;
    }
}

ModuleWidget *DatetimeModule::moduleWidget()
{
    if(!m_datetimeWidget)
    {
        m_datetimeWidget = new Datetime();
        connect(m_datetimeWidget, SIGNAL(editDatetime()), this, SLOT(slotEditDatetime()));
    }

    return m_datetimeWidget;
}

const QString DatetimeModule::name() const
{
    return "Datetime";
}

void DatetimeModule::contentPopped(ContentWidget * const w)
{
    if (w == m_dateSettings)
        m_dateSettings = nullptr;

    w->deleteLater();
}

void DatetimeModule::slotEditDatetime()
{
    if(!m_dateSettings)
    {
        m_dateSettings = new DateSettings();

        m_dateSettings->setNTP(m_model->nTP());
        connect(m_dateSettings, SIGNAL(autoSyncChanged(bool)), m_work, SLOT(setNTP(bool)));
        connect(m_dateSettings,SIGNAL(dateChanged(int,int,int,int,int)), m_work, SLOT(setDatetime(int,int,int,int,int)));
        connect(m_model, SIGNAL(NTPChanged(bool)), m_dateSettings, SLOT(slotAutoSync(bool)));
    }

    m_frameProxy->pushWidget(this, m_dateSettings);
}
