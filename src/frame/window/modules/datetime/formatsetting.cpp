/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     niecheng <niecheng@uniontech.com>
*
* Maintainer: niecheng <niecheng@uniontech.com>
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

#include "formatsetting.h"
#include "modules/datetime/datetimemodel.h"
#include "modules/datetime/timezoneitem.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "window/utils.h"
#include "currencyformat.h"
#include "numberformat.h"

#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QComboBox>
#include <QDBusInterface>

using namespace dcc::datetime;
using namespace DCC_NAMESPACE::datetime;
using namespace dcc::widgets;

FormatSetting::FormatSetting(DatetimeModel *mdoel, QWidget *parent)
    : ContentWidget(parent)
    , m_layout(new QVBoxLayout)
    , mModel(mdoel)
    , m_currencyFormatWidget(new CurrencyFormat(mModel))
    , m_numberFormatWidget(new NumberFormat(mModel))
{
    setAccessibleName("FormatSetting");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    TitleLabel *headTitle = new TitleLabel(tr("Date and Time Formats"));     //时间格式设置
    m_layout->setSpacing(10);
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addWidget(headTitle, 0, Qt::AlignLeft);

    SettingsGroup *timeGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    timeGrp->layout()->setContentsMargins(0, 0, 0, 0);

    m_weekCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_weekCbx->setTitle(tr("Weeks"));   //星期

    m_shortDateCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_shortDateCbx->setTitle(tr("Short Date"));  //短日期

    m_longdateCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_longdateCbx->setTitle(tr("Long Date"));  //长日期

    m_shortimeCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_shortimeCbx->setTitle(tr("Short Time"));  //短时间

    m_longtimeCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_longtimeCbx->setTitle(tr("Long Time"));  //长时间

    m_weekStartDayCbx = new ComboxWidget();
    //~ contents_path /datetime/Time Format
    //~ child_page Time Format
    m_weekStartDayCbx->setTitle(tr("First Day of Week"));  //长时间

    m_weekCbx->setVisible(QLocale::system().name().contains("zh"));

    timeGrp->appendItem(m_weekCbx);
    timeGrp->appendItem(m_weekStartDayCbx);
    timeGrp->appendItem(m_longdateCbx);
    timeGrp->appendItem(m_shortDateCbx);
    timeGrp->appendItem(m_longtimeCbx);
    timeGrp->appendItem(m_shortimeCbx);

    m_layout->addWidget(timeGrp);
    m_layout->addWidget(m_currencyFormatWidget);
    m_layout->addWidget(m_numberFormatWidget);

    initComboxWidgetList();
    m_layout->addStretch(0);

    QWidget *widget = new QWidget;
    widget->setLayout(m_layout);
    setContent(widget);
}

/**
 * @brief FormatSetting::initComboxWidgetList 初始化下拉列表的内容
 */
void FormatSetting::initComboxWidgetList()
{
    int formatcount = 2;
    for (int i = 0; i < formatcount; i++) {
        m_weekCbx->comboBox()->addItem(fotmatWeek(i));
    }

    int weekbeginscount = 7;
    for (int i = 0; i < weekbeginscount; i++) {
        m_weekStartDayCbx->comboBox()->addItem(weekStartWithDay(i));
    }

    int shortdatecount = 11;
    for (int i = 0; i < shortdatecount; i++) {
        m_shortDateCbx->comboBox()->addItem(fotmatShortDate(i));
    }

    int longdatecount = 3;
    for (int i = 0; i < longdatecount; i++) {
        m_longdateCbx->comboBox()->addItem(fotmatLongDate(i));
    }

    int longtimecount = 2;
    for (int i = 0; i < longtimecount; i++) {
        m_longtimeCbx->comboBox()->addItem(fotmatLongTime(i));
    }

    int shorttimecount = 2;
    for (int i = 0; i < shorttimecount; i++) {
        m_shortimeCbx->comboBox()->addItem(fotmatShortTime(i));
    }

    m_shortimeCbx->comboBox()->setCurrentIndex(mModel->shorTimeFormat());
    m_weekCbx->comboBox()->setCurrentIndex(mModel->weekdayFormatType());
    m_shortDateCbx->comboBox()->setCurrentIndex(mModel->shortDateFormat());
    m_longdateCbx->comboBox()->setCurrentIndex(mModel->longDateFormat());
    m_longtimeCbx->comboBox()->setCurrentIndex(mModel->longTimeFormat());
    m_weekStartDayCbx->comboBox()->setCurrentIndex(mModel->weekStartDayFormat());

    connect(m_weekCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::weekdayFormatChanged);
    connect(m_shortDateCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::shortDateFormatChanged);
    connect(m_longdateCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::longDateFormatChanged);
    connect(m_longtimeCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::longTimeFormatChanged);
    connect(m_shortimeCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::shortTimeFormatChanged);
    connect(m_weekStartDayCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &FormatSetting::weekStartDayFormatChanged);

    connect(m_currencyFormatWidget, &CurrencyFormat::positiveCurrencyFormatChanged, m_numberFormatWidget, &NumberFormat::SetPositiveCurrencyFormat);
    connect(m_currencyFormatWidget, &CurrencyFormat::negativeCurrencyChanged, m_numberFormatWidget, &NumberFormat::SetNegativeCurrency);
    m_numberFormatWidget->SetPositiveCurrencyFormat(m_currencyFormatWidget->getFirstPositiveCurrencyFormatPlace());
    m_numberFormatWidget->SetNegativeCurrency(m_currencyFormatWidget->getFirstNegativeCurrencyPlace());
}

QString FormatSetting::fotmatWeek(int type)
{
    QString day("");
    switch (type) {
    case 0:
        day = tr("Monday"); //星期一
        break;
    case 1:
        day = tr("monday"); //周一
        break;
    default:
        day = tr("Monday"); //星期一
        break;
    }
    return day;
}

QString FormatSetting::weekStartWithDay(int type)
{
    QString day("");
    switch (type) {
    case 0:
        day = tr("Monday"); //星期一
        break;
    case 1:
        day = tr("Tuesday"); //星期二
        break;
    case 2:
        day = tr("Wednesday"); //星期三
        break;
    case 3:
        day = tr("Thursday"); //星期四
        break;
    case 4:
        day = tr("Friday"); //星期五
        break;
    case 5:
        day = tr("Saturday"); //星期六
        break;
    case 6:
        day = tr("Sunday"); //星期天
        break;
    default:
        day = tr("Monday"); //星期一
        break;
    }
    return day;
}
/**
 * @brief FormatSetting::fotmatLongDate
 * @param type
 * @return  返回当前类型长日期的格式
 */
QString FormatSetting::fotmatLongDate(int type)
{
    QString date("");
    switch (type) {
    case 0:
        date = tr("April 5, 2020");
        break;
    case 1:
        date = tr("April 5, 2020, Sunday");
        break;
    case 2:
        date = tr("Sunday, April 5, 2020");
        break;
    default:
        date = tr("April 5, 2020");
        break;
    }
    return date;
}

/**
 * @brief FormatSetting::fotmatShortDate
 * @param type
 * @return  返回当前类型对应的短日期格式
 */
QString FormatSetting::fotmatShortDate(int type)
{
    QString date("");
    switch (type) {
    case 0:
        date = tr("2020/4/5");
        break;
    case 1:
        date = tr("2020-4-5");
        break;
    case 2:
        date = tr("2020.4.5");
        break;
    case 3:
        date = tr("2020/04/05");
        break;
    case 4:
        date = tr("2020-04-05");
        break;
    case 5:
        date = tr("2020.04.05");
        break;
    case 6:
        date = tr("04.05.2020");
        break;
    case 7:
        date = tr("05.04.2020");
        break;
    case 8:
        date = tr("20/4/5");
        break;
    case 9:
        date = tr("20-4-5");
        break;
    case 10:
        date = tr("20.4.5");
        break;
    default:
        date = tr("2020/04/05");
        break;
    }
    return date;
}

/**
 * @brief FormatSetting::fotmatLongTime
 * @param type
 * @return 返回该类型长时间的格式
 */
QString FormatSetting::fotmatLongTime(int type)
{
    QString time("");
    switch (type) {
    case 0:
        time = tr("9:40:07");
        break;
    case 1:
        time = tr("09:40:07");
        break;
    default:
        time = tr("09:40:07");
        break;
    }
    return time;
}

/**
 * @brief FormatSetting::fotmatShortTime
 * @param type
 * @return 返回当前短时间的格式
 */
QString FormatSetting::fotmatShortTime(int type)
{
    QString time("");
    switch (type) {
    case 0:
        time = tr("9:40");
        break;
    case 1:
        time = tr("09:40");
        break;
    default:
        time = tr("09:40");
        break;
    }
    return time;

}

/**
 * @brief FormatSetting::setCururentWeekdayFormat
 * @param type 设置当前星期的格式
 */
void FormatSetting::setCururentWeekdayFormat(int type)
{
    if (m_weekCbx && m_weekCbx->comboBox()->count() > type) {
        m_weekCbx->blockSignals(true);
        m_weekCbx->comboBox()->setCurrentIndex(type);
        m_weekCbx->blockSignals(false);
    }
}

/**
 * @brief FormatSetting::setCururentShortDateFormat
 * @param type 设置i当前短日期的格式
 */
void FormatSetting::setCururentShortDateFormat(int type)
{
    if (m_shortDateCbx && m_shortDateCbx->comboBox()->count() > type) {
        m_shortDateCbx->blockSignals(true);
        m_shortDateCbx->comboBox()->setCurrentIndex(type);
        m_shortDateCbx->blockSignals(false);
    }
}

/**
 * @brief FormatSetting::setCururentLongDateFormat
 * @param type 当前设置长日期的显示类型
 */
void FormatSetting::setCururentLongDateFormat(int type)
{
    if (m_longdateCbx && m_longdateCbx->comboBox()->count() > type) {
        m_longdateCbx->blockSignals(true);
        m_longdateCbx->comboBox()->setCurrentIndex(type);
        m_longdateCbx->blockSignals(false);
    }
}

/**
 * @brief FormatSetting::setCururentLongTimeFormat
 * @param type 当前设置长时间的显示类型
 */
void FormatSetting::setCururentLongTimeFormat(int type)
{
    if (m_longtimeCbx && m_longtimeCbx->comboBox()->count() > type) {
        m_longtimeCbx->blockSignals(true);
        m_longtimeCbx->comboBox()->setCurrentIndex(type);
        m_longtimeCbx->blockSignals(false);
    }
}
/**
 * @brief FormatSetting::setCururentShortTimeFormat
 * @param type 当前设置短时间的显示类型
 */
void FormatSetting::setCururentShortTimeFormat(int type)
{
    if (m_shortimeCbx && m_shortimeCbx->comboBox()->count() > type) {
        m_shortimeCbx->blockSignals(true);
        m_shortimeCbx->comboBox()->setCurrentIndex(type);
        m_shortimeCbx->blockSignals(false);
    }
}

void FormatSetting::setCururentWeekStartDayFormat(int type)
{
    if (m_weekStartDayCbx && m_weekStartDayCbx->comboBox()->count() > type) {
        m_weekStartDayCbx->blockSignals(true);
        m_weekStartDayCbx->comboBox()->setCurrentIndex(type);
        m_weekStartDayCbx->blockSignals(false);
    }
}

