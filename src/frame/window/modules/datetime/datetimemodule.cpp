/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "datetimemodule.h"
#include "datetimewidget.h"
#include "timezonelist.h"
#include "datesettings.h"
#include "clockitem.h"
#include "systemtimezone.h"
#include "formatsetting.h"
#include "window/gsettingwatcher.h"
#include "window/mainwindow.h"

#include <types/zoneinfo.h>

#include "modules/datetime/datetimework.h"
#include "modules/datetime/datetimemodel.h"
#include "modules/datetime/timezone_dialog/timezone.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"

using namespace dcc::datetime;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeModule::DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_work(nullptr)
    , m_model(nullptr)
    , m_timezonelist(nullptr)
    , m_widget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("timezoneList");
    GSettingWatcher::instance()->insertState("timeSettings");
    GSettingWatcher::instance()->insertState("timeFormat");
}

void DatetimeModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    Q_UNUSED(sync)

    m_work = &DatetimeWork::getInstance();
    m_model = m_work->model();

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

    initSearchData();
}

void DatetimeModule::initialize()
{
#ifndef DCC_DISABLE_TIMEZONE
    connect(this, &DatetimeModule::requestSetTimeZone, m_work, &DatetimeWork::setTimezone);
    connect(this, &DatetimeModule::requestRemoveUserTimeZone, m_work, &DatetimeWork::removeUserTimeZone);
    connect(this, &DatetimeModule::requestAddUserTimeZone, m_work, &DatetimeWork::addUserTimeZone);
#endif
    connect(this, &DatetimeModule::weekdayFormatChanged, m_work, &DatetimeWork::setWeekdayFormat);
    connect(this, &DatetimeModule::shortDateFormatChanged, m_work, &DatetimeWork::setShortDateFormat);
    connect(this, &DatetimeModule::longDateFormatChanged, m_work, &DatetimeWork::setLongDateFormat);
    connect(this, &DatetimeModule::longTimeFormatChanged, m_work, &DatetimeWork::setLongTimeFormat);
    connect(this, &DatetimeModule::shortTimeFormatChanged, m_work, &DatetimeWork::setShortTimeFormat);
    connect(this, &DatetimeModule::weekStartDayFormatChanged, m_work, &DatetimeWork::setWeekStartDayFormat);
}

const QString DatetimeModule::name() const
{
    return QStringLiteral("datetime");
}

const QString DatetimeModule::displayName() const
{
    return tr("Date and Time");
}

void DatetimeModule::active()
{
    m_widget = new DatetimeWidget;
    m_widget->setVisible(false);
    connect(m_widget, &DatetimeWidget::requestTimeSetting, this, &DatetimeModule::showTimeSetting);
    connect(m_widget, &DatetimeWidget::requestTimezoneList, this, &DatetimeModule::showTimezoneList);
    connect(m_widget, &DatetimeWidget::requestFormatSetting, this, &DatetimeModule::showFormatSetting);
    connect(m_widget, &DatetimeWidget::requestSetHourType, m_work, &DatetimeWork::set24HourType);
    connect(m_model,  &DatetimeModel::hourTypeChanged, m_widget, &DatetimeWidget::onHourTypeChanged);
    connect(m_widget, &DatetimeWidget::requestCloseWidget, this, &DatetimeModule::closeDialog);
    connect(m_widget, &DatetimeWidget::requestUpdateSecondMenu, this, [=](bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
            m_frameProxy->popWidget(this);
        m_widget->setDefaultWidget();
    });
    m_widget->setModel(m_model);
    m_work->activate(); //refresh data
    m_widget->setCurrentTimeZone(m_model->currentTimeZone());

    //set dbus data to 24 hour format
    m_widget->onHourTypeChanged(m_model->get24HourFormat());
    m_widget->setTimeType(QLocale::system().name());

    m_frameProxy->pushWidget(this, m_widget);
    m_widget->setVisible(true);
    m_widget->setDefaultWidget();
}

int DatetimeModule::load(const QString &path)
{
    if (!m_widget) {
        active();
    }

    QListView *list = m_widget->getListViewPointer();
    DatetimeType type = Default;

    if (!list) {
        return 0;
    }

    if (path == "Timezone List") {
        type = ETimezoneList;
    } else if (path == "Timezone List/Change System Timezone") {
        type = ESystemTimezone;
    } else if (path == "Time Settings") {
        type = TimeSetting;
    } else if (path == "Timezone List/Add Timezone") {
        type = AddTimeZone;
    } else if (path == "Format Settings") {
        type = FormatSetting;
    }

    QModelIndex index = list->model()->index(type, 0);
    switch (type) {
    case ETimezoneList:
    case TimeSetting:
        list->setCurrentIndex(index);
        list->clicked(index);
        break;
    case ESystemTimezone:
        //First enter timezoneList
        index = list->model()->index(ETimezoneList, 0);
        list->setCurrentIndex(index);
        list->clicked(index);

        //Then enter systemTimezone
        showSystemTimezone();
        break;
    case AddTimeZone:
        //First enter timezoneList
        index = list->model()->index(ETimezoneList, 0);
        list->setCurrentIndex(index);
        list->clicked(index);

        //Then enter addTimezone
        showSystemTimezone();
        Q_EMIT m_timezonelist->requestAddTimeZone();
        break;
    case FormatSetting:
        //First enter FormatSetting
        index = list->model()->index(FormatSetting, 0);
        list->setCurrentIndex(index);
        list->clicked(index);

        //Then enter FormatSetting
        showFormatSetting();
        break;
    default:
        break;
    }

    return 0;
}

QStringList DatetimeModule::availPage() const
{
    QStringList list;
    list << "Timezone List" << "Timezone List/Change System Timezone" << "Time Settings" << "Timezone List/Add Timezone" << "Format Settings";

    return list;
}

void DatetimeModule::initSearchData()
{
    static QMap<QString, bool> gsettingsMap;
    QString module = tr("Date and Time");
    QString timeSettings = tr("Time Settings");
    QString timezoneList = tr("Timezone List");
    QString formatSettings = tr("Format Settings");
    QString weeks = tr("Weeks").remove('/').trimmed();

    QStringList gsecondList;
    gsecondList << "timezoneList" << "timeSettings" << "timeFormat";

    auto func_is_visible = [ = ](const QString &gsettings, bool state = false) {
        if (gsettings == "") {
            return false;
        }

        bool ret = false;
        if (state) {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }

        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_timezonelist_changed = [ = ]() {
        if (!m_frameProxy || !m_model) {
            return;
        }

        bool bTimezonelist = func_is_visible("timezoneList", true);
        m_frameProxy->setWidgetVisible(module, timezoneList, bTimezonelist);
        m_frameProxy->setDetailVisible(module, timezoneList, tr("Change System Timezone"), bTimezonelist);
        m_frameProxy->setDetailVisible(module, timezoneList, tr("System Timezone"), bTimezonelist);
        m_frameProxy->setDetailVisible(module, timezoneList, tr("Add Timezone"), bTimezonelist && func_is_visible("datetimeZonelistAddtimezone"));
    };

    auto func_format_changed = [ = ]() {
        if (!m_frameProxy || !m_model) {
            return;
        }

        //TODO: datetimeFromatsetting为Hidden时只会隐藏三级页面，耳机页面的选项还在需要移除
        bool bTimeFormat = func_is_visible("timeFormat", true) && func_is_visible("datetimeFromatsetting");
        m_frameProxy->setWidgetVisible(module, formatSettings, bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("Date and Time Formats"), bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, weeks, bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("Short Date"), bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("Long Date"), bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("Short Time"), bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("Long Time"), bTimeFormat);
        m_frameProxy->setDetailVisible(module, formatSettings, tr("First Day of Week"), bTimeFormat);
    };

    auto func_setting_changed = [ = ]() {
        if (!m_frameProxy || !m_model) {
            return;
        }

        bool bTimeSettings = func_is_visible("timeSettings", true);
        m_frameProxy->setWidgetVisible(module, timeSettings, bTimeSettings);
        m_frameProxy->setDetailVisible(module, timeSettings, tr("Auto Sync"), bTimeSettings && func_is_visible("datetimeDatesettingAutosync"));
        m_frameProxy->setDetailVisible(module, timeSettings, tr("Server"), bTimeSettings && m_model->nTP());
    };

    auto func_process_all = [ = ]() {

        m_frameProxy->setModuleVisible(module, true);
        m_frameProxy->setWidgetVisible(module, tr("24-hour Time"), true);

        func_timezonelist_changed();

        func_format_changed();

        func_setting_changed();

     };

    //更新开/关时间同步显示“服务器”
    connect(m_model, &DatetimeModel::NTPChanged, [ = ](bool value) {
        m_frameProxy->setDetailVisible(module, timeSettings, tr("Server"), func_is_visible("timeSettings", true) && value);
        if (m_frameProxy) {
            m_frameProxy->updateSearchData(module);
        }
    });

    //gsetting data changed
    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsecondList.contains(gsetting)) {//bool: timezonlist, timeFormat, timeSettings
            if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
                return;
            }
        } else {//QString: datetimeZonelistAddtimezone, datetimeDatesettingAutosync, datetimeFromatsetting
            if (gsettingsMap.value(gsetting) == (state != "Hidden")) {
                return;
            }
        }

        if ("timezoneList" == gsetting) {
            func_timezonelist_changed();
        } else if ("timeSettings" == gsetting) {
            func_setting_changed();
        } else if ("timeFormat" == gsetting || "datetimeFromatsetting" == gsetting) {
            func_format_changed();
        } else if ("datetimeZonelistAddtimezone" == gsetting) {
            if (m_frameProxy)
                m_frameProxy->setDetailVisible(module, timezoneList, tr("Add Timezone"), func_is_visible("timezoneList") && func_is_visible("datetimeZonelistAddtimezone"));
        } else if ("datetimeDatesettingAutosync" == gsetting) {
            if (m_frameProxy)
                m_frameProxy->setDetailVisible(module, timeSettings, tr("Auto Sync"), func_is_visible("timeSettings", true) && func_is_visible("datetimeDatesettingAutosync"));
        } else {
            qInfo() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void DatetimeModule::updateSystemTimezone(const QString &timezone)
{
    if (timezone.isEmpty()) return;

    const QString locale = QLocale::system().name();
    const QString name = installer::GetLocalTimezoneName(timezone, locale);

    if (m_dialog) {
        m_dialog->setCurrentTimeZoneText(name);
        m_dialog->close();
    }

    if (m_timezonelist && m_model) {
        m_timezonelist->getTimezoneContentListPtr()->updateTimezones(m_model->userTimeZones());
    }

    if (!m_widget.isNull()) {
        m_widget->setCurrentTimeZone(m_model->currentTimeZone());
    }
}

void DatetimeModule::ensureZoneChooserDialog()
{
    if (m_dialog)
        return;

    m_dialog = new TimeZoneChooser(m_pMainWindow);
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_dialog, &TimeZoneChooser::confirmed, this, [this](const QString & timezone) {
        if (m_dialog->isAddZone()) {
            Q_EMIT requestAddUserTimeZone(timezone);
        } else {
            Q_EMIT requestSetTimeZone(timezone);
        }

        m_dialog->close();
    });

    connect(m_model, &dcc::datetime::DatetimeModel::systemTimeZoneIdChanged, this, &DatetimeModule::updateSystemTimezone);
}

void DatetimeModule::showFormatSetting()
{
    m_fsetting = new DCC_NAMESPACE::datetime::FormatSetting(m_model);
    connect(m_fsetting, &FormatSetting::weekdayFormatChanged, this, &DatetimeModule::weekdayFormatChanged);
    connect(m_fsetting, &FormatSetting::shortDateFormatChanged, this, &DatetimeModule::shortDateFormatChanged);
    connect(m_fsetting, &FormatSetting::longDateFormatChanged, this, &DatetimeModule::longDateFormatChanged);
    connect(m_fsetting, &FormatSetting::longTimeFormatChanged, this, &DatetimeModule::longTimeFormatChanged);
    connect(m_fsetting, &FormatSetting::shortTimeFormatChanged, this, &DatetimeModule::shortTimeFormatChanged);
    connect(m_fsetting, &FormatSetting::weekStartDayFormatChanged, this, &DatetimeModule::weekStartDayFormatChanged);
    connect(m_model, &DatetimeModel::weekdayFormatTypeChanged, m_fsetting, &FormatSetting::setCururentWeekdayFormat);
    connect(m_model, &DatetimeModel::shortDateFormatChanged, m_fsetting, &FormatSetting::setCururentShortDateFormat);
    connect(m_model, &DatetimeModel::longDateFormatChanged, m_fsetting, &FormatSetting::setCururentLongDateFormat);
    connect(m_model, &DatetimeModel::longTimeFormatChanged, m_fsetting, &FormatSetting::setCururentLongTimeFormat);
    connect(m_model, &DatetimeModel::shorTimeFormatChanged, m_fsetting, &FormatSetting::setCururentShortTimeFormat);
    connect(m_model, &DatetimeModel::weekStartDayFormatChanged, m_fsetting, &FormatSetting::setCururentWeekStartDayFormat);
    m_frameProxy->pushWidget(this, m_fsetting);
    GSettingWatcher::instance()->bind("datetimeFromatsetting", m_fsetting);
}

void DatetimeModule::showTimezoneList()
{
    SystemTimezone *sysTimezoneWidget = new SystemTimezone;
    connect(sysTimezoneWidget, &SystemTimezone::requestSetSystemTimezone, this, &DatetimeModule::showSystemTimezone);
    connect(m_model, &DatetimeModel::currentSystemTimeZoneChanged, sysTimezoneWidget, &SystemTimezone::setSystemTimezone);
    sysTimezoneWidget->setSystemTimezone(m_model->currentSystemTimeZone());

    if (!m_timezonelist) {
        m_timezonelist = new TimezoneList;

        //first into this page, update timezonelist data
        updateSystemTimezone(m_model->systemTimeZoneId());

        connect(m_model, &DatetimeModel::userTimeZoneAdded,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::addTimezone);
        connect(m_model, &DatetimeModel::userTimeZoneRemoved,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::removeTimezone);
        connect(m_timezonelist, &TimezoneList::requestAddTimeZone, this, [this] {
            ensureZoneChooserDialog();
            m_dialog->setIsAddZone(true);
            m_dialog->show();
        });
        connect(m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::requestRemoveUserTimeZone,
                this, &DatetimeModule::requestRemoveUserTimeZone);
        connect(m_timezonelist, &TimezoneList::requestAddUserTimeZone,
                this, &DatetimeModule::requestAddUserTimeZone);
    }

    QWidget *widget = new QWidget;
    widget->setAccessibleName("TimezoneList");
    widget->setVisible(false);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(sysTimezoneWidget, Qt::AlignTop);
    layout->addWidget(m_timezonelist);
    widget->setLayout(layout);
    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}

void DatetimeModule::showSystemTimezone()
{
    if (m_dialog && m_dialog->isVisible()) return;

    ensureZoneChooserDialog();
    m_dialog->setIsAddZone(false);
    m_dialog->show();
    if (!installer::GetCurrentTimezone().isEmpty()) {
        m_dialog->setMarkedTimeZone(installer::GetCurrentTimezone());
    } else {
        m_dialog->setMarkedTimeZone(m_model->getTimeZone());
    }
}

void DatetimeModule::showTimeSetting()
{
    m_setting = new DateSettings;
    m_setting->setVisible(false);
    connect(m_setting, &DateSettings::requestSetAutoSyncdate, m_work, &dcc::datetime::DatetimeWork::setNTP);
    connect(m_setting, &DateSettings::requestSetTime, m_work, &dcc::datetime::DatetimeWork::setDatetime);
    connect(m_setting, &DateSettings::requestBack, this, &DatetimeModule::onPopWidget);
    connect(m_setting, &DateSettings::requestNTPServer, [this](const QString server) {
        m_work->setNtpServer(server);
    });
    connect(m_model, &DatetimeModel::systemTimeChanged, m_setting, &DateSettings::requestBack);
    connect(m_model, &dcc::datetime::DatetimeModel::NTPChanged, m_setting, &DateSettings::updateRealAutoSyncCheckState);
    connect(m_model, &dcc::datetime::DatetimeModel::NTPServerListChanged, m_setting, &DateSettings::updateNTPServerList);
    connect(m_model, &DatetimeModel::NTPServerChanged, this, [this](QString server) {
        if (m_setting) {
            m_setting->setNtpServerAddress(server);
        }
    });
    connect(m_model, &DatetimeModel::NTPServerNotChanged, this, [this](QString server) {
        if (m_setting) {
            m_setting->setLastServerAddress(server);
        }
    });

    connect(m_model, &DatetimeModel::hourTypeChanged, m_setting, &DateSettings::updateSettingformatTime);
    //set dbus data to 24 hour format
    m_setting->updateSettingformatTime(m_model->get24HourFormat());

    m_setting->updateNTPServerList(m_model->ntpServerList());
    m_setting->setNtpServerAddress(m_model->ntpServerAddress());
    m_setting->updateRealAutoSyncCheckState(m_model->nTP());

    //fit the two clocks's point to same
    m_setting->setCurrentTimeZone(m_model->currentTimeZone());
    m_widget->setCurrentTimeZone(m_model->currentTimeZone());

    m_frameProxy->pushWidget(this, m_setting);
    m_setting->setVisible(true);
}

void DatetimeModule::onPopWidget()
{
    DListView *list = m_widget->getListViewPointer();

    if (!list) {
        qDebug() << " DListView is nullptr.";
        return;
    }

    QModelIndex index = list->model()->index(ETimezoneList, 0);
    list->setCurrentIndex(index);
    list->clicked(index);
}

void DatetimeModule::closeDialog()
{
    GSettingWatcher::instance()->erase("datetimeFromatsetting", m_fsetting);
    if (!m_dialog.isNull()) {
        m_dialog->close();
    }
}
