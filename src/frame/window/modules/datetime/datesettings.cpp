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
#include "timespinbox.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QDate>
#include <QSettings>
#include <QFontDatabase>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::datetime;

namespace DCC_NAMESPACE {
namespace datetime {

DateSettings::DateSettings(QWidget *parent)
    : QWidget(parent)
    , m_datetimeGroup(new SettingsGroup)
    , m_clock(new ClockItem(this, false))
    , m_autoSyncTimeSwitch(new SwitchWidget)
    , m_yearWidget(new DateWidget(DateWidget::Year, 1970, 9999))
    , m_monthWidget(new DateWidget(DateWidget::Month, 1, 12))
    , m_dayWidget(new DateWidget(DateWidget::Day, 1, 31))
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save, this))
    , m_bIsConfirmSetTime(false)
    , m_ntpServerList(nullptr)
    , m_ntpSrvItem(nullptr)
    , m_address(nullptr)
    , m_addressContent(nullptr)
    , m_ntpServerAddress("")
    , m_bIsUserOperate(false)
    , m_bSystemIsServer(false)
{
    QSettings setting("/etc/deepin-version", QSettings::IniFormat);
    setting.beginGroup("Release");
    m_bSystemIsServer = (setting.value("Type").toString() == "Server");

    if (m_bSystemIsServer) {
        m_ntpServerList = new datetimeCombox;
        m_ntpSrvItem = new SettingsItem;
        m_address = new SettingsItem;
        m_addressContent = new QLineEdit;
    }

    //~ contents_path /datetime/Time Settings
    m_autoSyncTimeSwitch->setTitle(tr("Auto Sync"));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //set clock plate to white
    m_clock->setPlate(false);

    QPushButton *cancelButton = m_buttonTuple->leftButton();
    QPushButton *confirmButton = m_buttonTuple->rightButton();

    //~ contents_path /datetime/Time Settings
    cancelButton->setText(tr("Cancel"));
    //~ contents_path /datetime/Time Settings
    confirmButton->setText(tr("Confirm"));

    cancelButton->setAccessibleName("TimeCancel");
    confirmButton->setAccessibleName("TimeConfirm");

    QDate date(QDate::currentDate());
    m_yearWidget->setValue(date.year());
    m_monthWidget->setValue(date.month());
    m_dayWidget->setValue(date.day());

    QLabel *centerLabel = new QLabel(" : ");
    QFont font;
    font.setPointSizeF(24);
    centerLabel->setFont(font);
    centerLabel->setContextMenuPolicy(Qt::NoContextMenu);

    QTime time(QTime::currentTime());
    m_timeHourWidget = createDSpinBox(this, 0, 23);
    m_timeMinWidget = createDSpinBox(this, 0, 59);
    m_timeHourWidget->setValue(time.hour());
    m_timeMinWidget->setValue(time.minute());
    m_timeHourWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_timeMinWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);

    int nIndex = QFontDatabase::addApplicationFont(":/datetime/resource/deepindigitaltimes-Regular.ttf");
    if (nIndex != -1) {
        QStringList strList(QFontDatabase::applicationFontFamilies(nIndex));
        if (strList.count() > 0) {
            QFont fontThis(strList.at(0));
            fontThis.setPointSize(28);
            m_timeHourWidget->setFont(fontThis);
            m_timeMinWidget->setFont(fontThis);
        }
    }

    SettingsItem *timeItem = new SettingsItem;
    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addStretch();
    timeLayout->addWidget(m_timeHourWidget);
    timeLayout->addWidget(centerLabel);
    timeLayout->addWidget(m_timeMinWidget);
    timeLayout->addStretch();
    timeItem->setLayout(timeLayout);

    if (m_bSystemIsServer) {
        QHBoxLayout *ntpServeLayout = new QHBoxLayout;
        //~ contents_path /datetime/Time Settings
        QLabel *serverText = new QLabel(tr("Server"));
        m_ntpSrvItem->setLayout(ntpServeLayout);
        ntpServeLayout->addSpacing(10);
        ntpServeLayout->addWidget(serverText, 0, Qt::AlignLeft);
        ntpServeLayout->addWidget(m_ntpServerList, 0, Qt::AlignRight);

        QHBoxLayout *ntpAddressLayout = new QHBoxLayout;
        //~ contents_path /datetime/Time Settings
        QLabel *addressText = new QLabel(tr("Address"));
        m_addressContent->setMinimumWidth(240);
        //~ contents_path /datetime/Time Settings
        m_addressContent->setPlaceholderText(tr("Required"));

        ntpAddressLayout->addSpacing(10);
        ntpAddressLayout->addWidget(addressText, 0, Qt::AlignLeft);
        ntpAddressLayout->addWidget(m_addressContent, 0, Qt::AlignRight);
        m_address->setLayout(ntpAddressLayout);

        m_ntpServerList->setMinimumWidth(240);
        //~ contents_path /datetime/Time Settings
        m_ntpServerList->addItem(tr("Customize"));
    }

    m_datetimeGroup->appendItem(timeItem);
    m_datetimeGroup->appendItem(m_autoSyncTimeSwitch);
    if (m_bSystemIsServer) {
        m_datetimeGroup->appendItem(m_ntpSrvItem);
        m_datetimeGroup->appendItem(m_address);
    }
    m_datetimeGroup->appendItem(m_yearWidget);
    m_datetimeGroup->appendItem(m_monthWidget);
    m_datetimeGroup->appendItem(m_dayWidget);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_clock);

    if (m_bSystemIsServer) {
        ContentWidget *contentWidget = new ContentWidget(this);
        contentWidget->setContent(m_datetimeGroup);
        layout->addWidget(contentWidget);

        connect(m_ntpServerList, &datetimeCombox::click, this, &DateSettings::isUserOperate);
        connect(m_ntpServerList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DateSettings::onProcessComboBox);
    } else {
        layout->addWidget(m_datetimeGroup);
    }

    layout->addStretch();
    layout->addWidget(m_buttonTuple, 0, Qt::AlignBottom);
    setLayout(layout);

    connect(m_autoSyncTimeSwitch, &SwitchWidget::checkedChanged, this, &DateSettings::requestSetAutoSyncdate);

    connect(cancelButton, &QPushButton::clicked, this, &DateSettings::onCancelButtonClicked);
    connect(confirmButton, &QPushButton::clicked, this, &DateSettings::onConfirmButtonClicked);

    connect(m_monthWidget, &DateWidget::editingFinished, this, &DateSettings::updateDayRange);
    connect(m_yearWidget, &DateWidget::editingFinished, this, &DateSettings::updateDayRange);
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
    if (m_autoSyncTimeSwitch->checked()) {//1 -> 0
        Q_EMIT requestSetAutoSyncdate(false);

        //wait sync response, then set datetime
        if (!m_bIsConfirmSetTime) {
            m_bIsConfirmSetTime = true;
        }
    } else {
        Q_EMIT requestSetTime(getDatetime());
    }

    if (m_bSystemIsServer && m_ntpServerList->currentText() == tr("Customize")) {
        Q_EMIT requestNTPServer(m_addressContent->text());
    }
}

void DateSettings::updateDayRange()
{
    const int year = m_yearWidget->value();
    const int month = m_monthWidget->value();

    QDate date(year, month, 1);
    m_dayWidget->setRange(1, date.daysInMonth());

    if (m_dayWidget->maximum() < m_dayWidget->getCurrentText().toInt()) {
        m_dayWidget->setCurrentText(QString(m_dayWidget->maximum()));
    }
}

void DateSettings::onProcessComboBox(const int &value)
{
    if (!m_bSystemIsServer || !m_ntpServerList || m_ntpServerList->count() <= value)
        return;

    QString itemText = m_ntpServerList->itemText(value);
    m_address->setVisible(itemText == tr("Customize"));

    if (!m_bIsUserOperate)
        return;

    m_bIsUserOperate = false;

    if (itemText != tr("Customize") && "" != itemText) {
        Q_EMIT requestNTPServer(itemText);
    }
}

void DateSettings::isUserOperate()
{
    if (!m_bSystemIsServer)
        return;

    if (!m_bIsUserOperate) {
        m_bIsUserOperate = true;
    }
}

QDateTime DateSettings::getDatetime() const
{
    QDate date;
    date.setDate(m_yearWidget->value(), m_monthWidget->value(), m_dayWidget->value());
    QTime time;
    time.setHMS(m_timeHourWidget->text().toInt(), m_timeMinWidget->text().toInt(), 0);
    QDateTime datetime(date, time);

    return datetime;
}

void DateSettings::setNtpServerAddress(QString address)
{
    if (!m_bSystemIsServer)
        return;

    if (m_ntpServerAddress != address) {
        m_ntpServerAddress = address;

        //Set system ntp server address
        for (int i = 0; i < m_ntpServerList->count(); i++) {
            if (m_ntpServerList->itemText(i) == address) {
                m_ntpServerList->setCurrentText(address);
                return;
            }
        }

        //Set customize ntp server address
        m_ntpServerList->setVisible(true);
        m_addressContent->setVisible(true);

        m_ntpServerList->setCurrentText(tr("Customize"));
        m_addressContent->setText(address);
    }
}

QSpinBox *DateSettings::createDSpinBox(QWidget *parent, int min, int max)
{
    TimeSpinBox *spinBox = new TimeSpinBox(parent);
    spinBox->setFixedSize(93, 60);
    spinBox->setRange(min, max);
    spinBox->setSingleStep(1);
    spinBox->setWrapping(true);
    spinBox->setValue(0);

    DIconButton *btnUp = new DIconButton(spinBox);
    DIconButton *btnDown = new DIconButton(spinBox);
    btnUp->setIcon(DStyle::SP_ArrowUp);
    btnDown->setIcon(DStyle::SP_ArrowDown);
    btnUp->setFixedSize(QSize(SpinBtnLength, SpinBtnLength));
    btnDown->setFixedSize(QSize(SpinBtnLength, SpinBtnLength));
    btnUp->move(57, 4);
    btnDown->move(57, 31);

    connect(btnUp, &DIconButton::clicked, spinBox, &QSpinBox::stepUp);
    connect(btnDown, &DIconButton::clicked, spinBox, &QSpinBox::stepDown);

    return spinBox;
}

void DateSettings::updateRealAutoSyncCheckState(const bool &state)
{
    QDateTime datetime = getDatetime();

    if (m_autoSyncTimeSwitch->checked() != state) {
        m_autoSyncTimeSwitch->setChecked(state);

        //sync display current datetime
        m_yearWidget->setValue(datetime.date().year());
        m_monthWidget->setValue(datetime.date().month());
        m_dayWidget->setValue(datetime.date().day());
        m_timeHourWidget->setValue(datetime.time().hour());
        m_timeMinWidget->setValue(datetime.time().minute());
    }

    if (m_bIsConfirmSetTime && !state) {
        m_bIsConfirmSetTime = false;

        Q_EMIT requestSetTime(datetime);
    }
}

void DateSettings::updateNTPServerList(const QStringList list)
{
    if (!m_bSystemIsServer)
        return;

    if (m_ntpServerList && m_ntpServerList->count() > 0) {
        m_ntpServerList->clear();
    }
    m_ntpServerList->addItems(list);
    m_ntpServerList->addItem(tr("Customize"));
}

}// namespace datetime
}// namespace DCC_NAMESPACE
