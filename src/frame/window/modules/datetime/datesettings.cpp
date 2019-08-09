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

#include "datesettings.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QDate>

#include "clock.h"
#include "datetimewidget.h"
#include "modules/datetime/datetimemodel.h"

#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/translucentframe.h"
#include "clockitem.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"
#include "widgets/buttontuple.h"
#include "datewidget.h"
#include "timewidget.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::datetime;

namespace DCC_NAMESPACE {
namespace datetime {

DateSettings::DateSettings(QWidget *parent)
    : QWidget(parent),
      m_datetimeGroup(new SettingsGroup),
      m_clock(new ClockItem(this, false)),
      m_autoSyncTimeSwitch(new SwitchWidget(tr("Auto Sync"))),
      m_timeHourWidget(new TimeWidget),
      m_timeMinWidget(new TimeWidget(nullptr, false)),
      m_yearWidget(new DateWidget(DateWidget::Year, 1970, 9999)),
      m_monthWidget(new DateWidget(DateWidget::Month, 1, 12)),
      m_dayWidget(new DateWidget(DateWidget::Day, 1, 31)),
      m_buttonTuple(new ButtonTuple)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPushButton *cancelButton = m_buttonTuple->leftButton();
    QPushButton *confirmButton = m_buttonTuple->rightButton();

    cancelButton->setText(tr("Cancel"));
    confirmButton->setText(tr("Confirm"));

    cancelButton->setAccessibleName("TimeCancel");
    confirmButton->setAccessibleName("TimeConfirm");

    QDate date(QDate::currentDate());
    m_yearWidget->setValue(date.year());
    m_monthWidget->setValue(date.month());
    m_dayWidget->setValue(date.day());

    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(0);
    layout->setMargin(0);

//    m_autoSyncTimeSwitch->setChecked(true);

    QFont font;
    font.setPointSizeF(24);

    QLabel *label = new QLabel(" : ");
    label->setFont(font);
    label->setContextMenuPolicy(Qt::NoContextMenu);

    QLabel *PlaceholderLeft = new QLabel;
    PlaceholderLeft->setFixedWidth(80);

    QLabel *PlaceholderRight = new QLabel;
    PlaceholderRight->setFixedWidth(80);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(PlaceholderLeft);
    timeLayout->addWidget(m_timeHourWidget);
    timeLayout->addWidget(label);
    timeLayout->addWidget(m_timeMinWidget);
    timeLayout->addWidget(PlaceholderRight);

    layout->addWidget(m_clock);
    layout->addLayout(timeLayout);

    m_datetimeGroup->appendItem(m_autoSyncTimeSwitch);
    m_datetimeGroup->appendItem(m_yearWidget);
    m_datetimeGroup->appendItem(m_monthWidget);
    m_datetimeGroup->appendItem(m_dayWidget);

    layout->addSpacing(10);
    layout->addWidget(m_datetimeGroup);
    layout->addSpacing(10);
    layout->addWidget(m_buttonTuple);

//    setContent(widget);
    setLayout(layout);

    connect(m_autoSyncTimeSwitch, &SwitchWidget::checkedChanged, this, &DateSettings::requestSetAutoSyncdate);

    connect(cancelButton, &QPushButton::clicked, this, &DateSettings::onCancelButtonClicked);
    connect(confirmButton, &QPushButton::clicked, this, &DateSettings::onConfirmButtonClicked);

    connect(m_monthWidget, &DateWidget::editingFinished, this, &DateSettings::updateDayRange);
}

void DateSettings::setCurrentTimeZone(const ZoneInfo &info)
{
    m_clock->setTimeZone(info);
}

void DateSettings::onCancelButtonClicked()
{
    Q_EMIT requestBack();
}

void DateSettings::onConfirmButtonClicked()
{
    QDate date; date.setDate(m_yearWidget->value(), m_monthWidget->value(), m_dayWidget->value());
    QTime time; time.setHMS(m_timeHourWidget->getEditValue(), m_timeMinWidget->getEditValue(), 0);

    QDateTime datetime(date, time);
    Q_EMIT requestSetTime(datetime);

    m_autoSyncTimeSwitch->setChecked(false);
    Q_EMIT requestBack();
}

void DateSettings::updateDayRange()
{
    const int year = m_yearWidget->value();
    const int month = m_monthWidget->value();

    QDate date(year, month, 1);
    m_dayWidget->setRange(1, date.daysInMonth());
}

void DateSettings::updateRealAutoSyncCheckState(const bool &state)
{
    if (m_autoSyncTimeSwitch->checked() != state) {
        m_autoSyncTimeSwitch->setChecked(state);
    }
}

}// namespace datetime
}// namespace DCC_NAMESPACE
