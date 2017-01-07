#include "datetimemodule.h"

namespace dcc {
namespace datetime {

DatetimeModule::DatetimeModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_datetimeWidget(nullptr),
      m_dateSettings(nullptr)
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
    m_work->activate();
}

void DatetimeModule::moduleDeactive()
{
    m_work->deactivate();
}

void DatetimeModule::reset()
{

}

ModuleWidget *DatetimeModule::moduleWidget()
{
    if(!m_datetimeWidget)
    {
        m_datetimeWidget = new Datetime();
        m_datetimeWidget->setModel(m_model);

        connect(m_datetimeWidget, &Datetime::requestSetNtp, m_work, &DatetimeWork::setNTP);
        connect(m_datetimeWidget, &Datetime::requestTimeSettings, this, &DatetimeModule::showTimeSettingsPage);
        connect(m_datetimeWidget, &Datetime::requestRemoveUserTimeZone, m_work, &DatetimeWork::removeUserTimeZone);
        connect(m_datetimeWidget, &Datetime::requestAddUserTimeZone, m_work, &DatetimeWork::addUserTimeZone);

        connect(m_datetimeWidget, &Datetime::requestHold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, false);
        });
        connect(m_datetimeWidget, &Datetime::requestUnhold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, true);
        });
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

void DatetimeModule::showTimeSettingsPage()
{
    if (!m_dateSettings) {
        m_dateSettings = new DateSettings;
        m_dateSettings->setModel(m_model);

        connect(m_dateSettings, &DateSettings::requestSetTime, m_work, &DatetimeWork::setDatetime);
        connect(m_dateSettings, &DateSettings::requestSetTimeZone, m_work, &DatetimeWork::setTimezone);

        connect(m_dateSettings, &DateSettings::requestHold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, false);
        });
        connect(m_dateSettings, &DateSettings::requestUnhold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, true);
        });
    }

    m_frameProxy->pushWidget(this, m_dateSettings);
}

DatetimeModule::~DatetimeModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if (m_datetimeWidget)
        m_datetimeWidget->deleteLater();
}

}
}
