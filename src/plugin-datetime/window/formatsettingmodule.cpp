/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "formatsettingmodule.h"
#include "datetimemodel.h"
#include "datetimeworker.h"
#include "widgets/widgetmodule.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/comboxwidget.h"

DCC_USE_NAMESPACE

FormatSettingModule::FormatSettingModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent)
    : ModuleObject("dcc_time_format", tr("Time Format"), parent)
    , m_model(model)
    , m_work(work)
    , m_fotmatWeek({ tr("Monday"), tr("monday") })
    , m_fotmatLongDate({ tr("April 5, 2020"), tr("April 5, 2020, Sunday"), tr("Sunday, April 5, 2020") })
    , m_fotmatShortDate({ tr("2020/4/5"), tr("2020-4-5"), tr("2020.4.5"), tr("2020/04/05"), tr("2020-04-05"), tr("2020.04.05"), tr("20/4/5"), tr("20-4-5"), tr("20.4.5") })
    , m_fotmatLongTime({ tr("9:40:07"), tr("09:40:07") })
    , m_fotmatShortTime({ tr("9:40"), tr("09:40") })
    , m_weekStartWithDay({ tr("Monday"), tr("Tuesday"), tr("Wednesday"), tr("Thursday"), tr("Friday"), tr("Saturday"), tr("Sunday") })
{
    deactive();
    setChildType(ModuleObject::ChildType::Page);

    appendChild(new WidgetModule<SwitchWidget>("24-hour Time", tr("24-hour Time"), [this](SwitchWidget *hourTypeSwitch) {
        hourTypeSwitch->setTitle(tr("24-hour Time"));
        hourTypeSwitch->setChecked(m_model->get24HourFormat());
        hourTypeSwitch->addBackground();
        // true : 24 hour type  ,  false : 12 hour type
        connect(hourTypeSwitch, &SwitchWidget::checkedChanged, m_work, &DatetimeWorker::set24HourType);
        connect(m_model, &DatetimeModel::hourTypeChanged, hourTypeSwitch, &SwitchWidget::setChecked);
    }));

    appendChild(new WidgetModule<SettingsGroup>("TimeFotmat", tr("Time Fotmat"), this, &FormatSettingModule::initTimeFotmat));
}

void FormatSettingModule::initTimeFotmat(DCC_NAMESPACE::SettingsGroup *timeGrp)
{
    timeGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    ComboxWidget *weekCbx = new ComboxWidget(tr("Weeks")); //星期
    weekCbx->setVisible(QLocale::system().name().contains("zh"));
    weekCbx->comboBox()->addItems(m_fotmatWeek);
    weekCbx->comboBox()->setCurrentIndex(m_model->weekdayFormatType());
    connect(m_model, &DatetimeModel::weekdayFormatTypeChanged, weekCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(weekCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setWeekdayFormat);

    ComboxWidget *weekStartDayCbx = new ComboxWidget(tr("First Day of Week")); //一周首日
    weekStartDayCbx->comboBox()->addItems(m_weekStartWithDay);
    weekStartDayCbx->comboBox()->setCurrentIndex(m_model->weekStartDayFormat());
    connect(m_model, &DatetimeModel::weekStartDayFormatChanged, weekStartDayCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(weekStartDayCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setWeekStartDayFormat);

    ComboxWidget *shortDateCbx = new ComboxWidget(tr("Short Date")); //短日期
    shortDateCbx->comboBox()->addItems(m_fotmatShortDate);
    shortDateCbx->comboBox()->setCurrentIndex(m_model->shortDateFormat());
    connect(m_model, &DatetimeModel::shortDateFormatChanged, shortDateCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(shortDateCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setShortDateFormat);

    ComboxWidget *longdateCbx = new ComboxWidget(tr("Long Date")); //长日期
    longdateCbx->comboBox()->addItems(m_fotmatLongDate);
    longdateCbx->comboBox()->setCurrentIndex(m_model->longDateFormat());
    connect(m_model, &DatetimeModel::longDateFormatChanged, longdateCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(longdateCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setLongDateFormat);

    ComboxWidget *shortimeCbx = new ComboxWidget(tr("Short Time")); //短时间
    shortimeCbx->comboBox()->addItems(m_fotmatShortTime);
    shortimeCbx->comboBox()->setCurrentIndex(m_model->shorTimeFormat());
    connect(m_model, &DatetimeModel::shorTimeFormatChanged, shortimeCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(shortimeCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setShortTimeFormat);

    ComboxWidget *longtimeCbx = new ComboxWidget(tr("Long Time")); //长时间
    longtimeCbx->comboBox()->addItems(m_fotmatLongTime);
    longtimeCbx->comboBox()->setCurrentIndex(m_model->longTimeFormat());
    connect(m_model, &DatetimeModel::longTimeFormatChanged, longtimeCbx->comboBox(), &QComboBox::setCurrentIndex);
    connect(longtimeCbx->comboBox(), QOverload<int>::of(&QComboBox::currentIndexChanged), m_work, &DatetimeWorker::setLongTimeFormat);

    timeGrp->appendItem(weekCbx);
    timeGrp->appendItem(weekStartDayCbx);
    timeGrp->appendItem(shortDateCbx);
    timeGrp->appendItem(longdateCbx);
    timeGrp->appendItem(shortimeCbx);
    timeGrp->appendItem(longtimeCbx);
}
