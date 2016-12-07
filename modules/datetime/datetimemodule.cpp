#include "datetimemodule.h"

DatetimeModule::DatetimeModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_datetimeWidget(nullptr),
      m_dateSettings(nullptr),
      m_choseDlg(new ChoseDialog())
{
}

void DatetimeModule::initialize()
{
    m_model = new DatetimeModel;
    m_work = new DatetimeWork(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void DatetimeModule::moduleActive()
{

}

void DatetimeModule::moduleDeactive()
{
//    if(m_datetimeWidget)
//    {
//        m_datetimeWidget->deleteLater();
//        m_datetimeWidget = nullptr;
    //    }
}

void DatetimeModule::reset()
{

}

ModuleWidget *DatetimeModule::moduleWidget()
{
    if(!m_datetimeWidget)
    {
        m_datetimeWidget = new Datetime();
        connect(m_datetimeWidget, SIGNAL(editDatetime()), this, SLOT(slotEditDatetime()));
        connect(m_choseDlg, SIGNAL(addTimezone(Timezone)), m_datetimeWidget, SLOT(addTimezone(Timezone)));
        connect(m_datetimeWidget, SIGNAL(addClick()), this, SLOT(onAddTimezoneClick()));
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
        connect(m_model, SIGNAL(NTPChanged(bool)), m_dateSettings, SLOT(slotAutoSync(bool)));
        connect(m_dateSettings, SIGNAL(autoSyncChanged(bool)), m_work, SLOT(setNTP(bool)));
        connect(m_dateSettings,SIGNAL(dateChanged(int,int,int,int,int)), m_work, SLOT(setDatetime(int,int,int,int,int)));
        connect(m_model, SIGNAL(NTPChanged(bool)), m_dateSettings, SLOT(slotAutoSync(bool)));
        connect(m_dateSettings, SIGNAL(changeClick()), this, SLOT(onChangeClick()));
        connect(m_choseDlg, SIGNAL(curTimezone(Timezone)), this, SLOT(onSetTimezone(Timezone)));
        connect(m_model, SIGNAL(timezoneChanged(QString)), m_dateSettings, SLOT(setTimezone(QString)));
    }

    m_frameProxy->pushWidget(this, m_dateSettings);
}

void DatetimeModule::onAddTimezoneClick()
{
    if(m_choseDlg)
    {
        m_choseDlg->setFlag(false);
        m_choseDlg->show();
    }
}

void DatetimeModule::onChangeClick()
{
    if(m_choseDlg)
    {
        m_choseDlg->setFlag(true);
        m_choseDlg->show();
    }
}

void DatetimeModule::onSetTimezone(const Timezone &tz)
{
    // 暂时不支持设置时区
    QTimeZone zone(tz.m_timezone.toStdString().c_str());
//    m_work->setTimezone(zone.id());
}

DatetimeModule::~DatetimeModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if (m_datetimeWidget)
        m_datetimeWidget->deleteLater();
    m_choseDlg->deleteLater();
}
