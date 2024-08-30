//SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "customregionformatdialog.h"

#include <DBackgroundGroup>
#include <DLabel>

using namespace DCC_NAMESPACE;

CustomRegionFormatDialog::CustomRegionFormatDialog(QWidget *parent)
    : DDialog(parent)
{
    setMinimumSize(540, 590);
    setTitle(tr("Custom Format"));

    QWidget *contentWidget = new QWidget;
    QVBoxLayout *mainVLayout = new QVBoxLayout(contentWidget);
    mainVLayout->setSpacing(0);
    mainVLayout->setMargin(0);

    QVBoxLayout *dateGrpHLayout = new QVBoxLayout;
    dateGrpHLayout->setContentsMargins(10, 6, 10, 6);
    dateGrpHLayout->setSpacing(0);
    DBackgroundGroup *dateGrp = new DBackgroundGroup;
    dateGrp->setLayout(dateGrpHLayout);
    dateGrp->setBackgroundRole(QPalette::Window);
    dateGrp->setItemSpacing(1);
    dateGrp->setUseWidgetBackground(false);

    QWidget *dayWidget = new QWidget;
    QHBoxLayout *dayHBoxLayout = new QHBoxLayout(dayWidget);
    dayHBoxLayout->setMargin(0);
    QLabel *dayLabel = new QLabel(tr("First day of week"));
    m_dayCombo = new QComboBox;
    dayHBoxLayout->addWidget(dayLabel);
    dayHBoxLayout->addWidget(m_dayCombo);

    QWidget *shortDateWidget = new QWidget;
    QHBoxLayout *shortDateHBoxLayout = new QHBoxLayout(shortDateWidget);
    shortDateHBoxLayout->setMargin(0);
    QLabel *shortDateLabel = new QLabel(tr("Short date"));
    m_shortDateCombo = new QComboBox;
    shortDateHBoxLayout->addWidget(shortDateLabel);
    shortDateHBoxLayout->addWidget(m_shortDateCombo);

    QWidget *longDateWidget = new QWidget;
    QHBoxLayout *longDateHBoxLayout = new QHBoxLayout(longDateWidget);
    longDateHBoxLayout->setMargin(0);
    QLabel *longDateLabel = new QLabel(tr("Long date"));
    m_longDateCombo = new QComboBox;
    longDateHBoxLayout->addWidget(longDateLabel);
    longDateHBoxLayout->addWidget(m_longDateCombo);

    dateGrpHLayout->addWidget(dayWidget);
    dateGrpHLayout->addWidget(shortDateWidget);
    dateGrpHLayout->addWidget(longDateWidget);

    QVBoxLayout *timeGrpHLayout = new QVBoxLayout;
    timeGrpHLayout->setContentsMargins(10, 6, 10, 6);
    timeGrpHLayout->setSpacing(0);
    DBackgroundGroup *timeGrp = new DBackgroundGroup;
    timeGrp->setBackgroundRole(QPalette::Window);
    timeGrp->setLayout(timeGrpHLayout);
    timeGrp->setItemSpacing(1);
    timeGrp->setUseWidgetBackground(false);

    QWidget *shortTimeWidget = new QWidget;
    QHBoxLayout *shortTimeHBoxLayout = new QHBoxLayout(shortTimeWidget);
    shortTimeHBoxLayout->setMargin(0);
    QLabel *shortTimeLabel = new QLabel(tr("Short time"));
    m_shortTimeCombo = new QComboBox;
    shortTimeHBoxLayout->addWidget(shortTimeLabel);
    shortTimeHBoxLayout->addWidget(m_shortTimeCombo);

    QWidget *longTimeWidget = new QWidget;
    QHBoxLayout *longTimeHBoxLayout = new QHBoxLayout(longTimeWidget);
    longTimeHBoxLayout->setMargin(0);
    QLabel *longTimeLabel = new QLabel(tr("Long time"));
    m_longTimeCombo = new QComboBox;
    longTimeHBoxLayout->addWidget(longTimeLabel);
    longTimeHBoxLayout->addWidget(m_longTimeCombo);

    timeGrpHLayout->addWidget(shortTimeWidget);
    timeGrpHLayout->addWidget(longTimeWidget);

    QVBoxLayout *otherGrpHLayout = new QVBoxLayout;
    otherGrpHLayout->setContentsMargins(10, 6, 10, 6);
    otherGrpHLayout->setSpacing(0);
    DBackgroundGroup *otherGrp = new DBackgroundGroup;
    otherGrp->setBackgroundRole(QPalette::Window);
    otherGrp->setLayout(otherGrpHLayout);
    otherGrp->setItemSpacing(1);
    otherGrp->setUseWidgetBackground(false);

    QWidget *currencyWidget = new QWidget;
    QHBoxLayout *currencyHBoxLayout = new QHBoxLayout(currencyWidget);
    currencyHBoxLayout->setMargin(0);
    QLabel *currencyLabel = new QLabel(tr("Currency symbol"));
    m_currencyValueLabel = new QLabel("$");
    m_currencyValueLabel->setAlignment(Qt::AlignRight);
    currencyHBoxLayout->addWidget(currencyLabel);
    currencyHBoxLayout->addWidget(m_currencyValueLabel);

    QWidget *numberWidget = new QWidget;
    QHBoxLayout *numberHBoxLayout = new QHBoxLayout(numberWidget);
    numberHBoxLayout->setMargin(0);
    QLabel *numberLabel = new QLabel(tr("Numbers"));
    m_numberValueLabel = new QLabel("123456789");
    m_numberValueLabel->setAlignment(Qt::AlignRight);
    numberHBoxLayout->addWidget(numberLabel);
    numberHBoxLayout->addWidget(m_numberValueLabel);

    QWidget *paperWidget = new QWidget;
    QHBoxLayout *paperHBoxLayout = new QHBoxLayout(paperWidget);
    paperHBoxLayout->setMargin(0);
    QLabel *paperLabel = new QLabel(tr("Paper"));
    m_paperValueLabel = new QLabel("A4");
    m_paperValueLabel->setAlignment(Qt::AlignRight);
    paperHBoxLayout->addWidget(paperLabel);
    paperHBoxLayout->addWidget(m_paperValueLabel);

    otherGrpHLayout->addWidget(currencyWidget);
    otherGrpHLayout->addWidget(numberWidget);
    otherGrpHLayout->addWidget(paperWidget);

    mainVLayout->addWidget(dateGrp);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(timeGrp);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(otherGrp);
    mainVLayout->addSpacing(40);

    addContent(contentWidget);

    addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    addButton(tr("Save"), true, DDialog::ButtonRecommend);

    connect(getButton(1), &QPushButton::clicked, this, &CustomRegionFormatDialog::onSaved);
}

CustomRegionFormatDialog::~CustomRegionFormatDialog()
{
}

void CustomRegionFormatDialog::initRegionFormat(const QLocale &locale, const RegionFormat &regionFormat)
{
    RegionAvailableData regionFormatsAvailable = RegionProxy::allTextData(locale);
    m_dayCombo->addItems(regionFormatsAvailable.daysAvailable);
    m_shortDateCombo->addItems(regionFormatsAvailable.shortDatesAvailable);
    m_longDateCombo->addItems(regionFormatsAvailable.longDatesAvailable);
    m_shortTimeCombo->addItems(regionFormatsAvailable.shortTimesAvailable);
    m_longTimeCombo->addItems(regionFormatsAvailable.longTimesAvailable);
    m_currencyValueLabel->setText(RegionProxy::regionFormat(locale).currencyFormat);
    m_numberValueLabel->setText(RegionProxy::regionFormat(locale).numberFormat);
    m_paperValueLabel->setText(RegionProxy::regionFormat(locale).paperFormat);

    m_dayCombo->setCurrentText(locale.standaloneDayName(regionFormat.firstDayOfWeekFormat));
    m_shortDateCombo->setCurrentText(locale.toString(QDate(2023, 1, 1), regionFormat.shortDateFormat));
    m_longDateCombo->setCurrentText(locale.toString(QDate(2023, 1, 1), regionFormat.longDateFormat));
    m_shortTimeCombo->setCurrentText(locale.toString(QTime(1, 1, 1), regionFormat.shortTimeFormat));
    m_longTimeCombo->setCurrentText(locale.toString(QTime(1, 1, 1), regionFormat.longTimeFormat));
}

void CustomRegionFormatDialog::onSaved()
{
    RegionAvailableData regionFormat = RegionProxy::allFormat();
    m_format.firstDayOfWeekFormat = m_dayCombo->currentIndex() + 1;
    m_format.shortDateFormat = regionFormat.shortDatesAvailable.at(m_shortDateCombo->currentIndex());
    m_format.longDateFormat = regionFormat.longDatesAvailable.at(m_longDateCombo->currentIndex());
    m_format.shortTimeFormat = regionFormat.shortTimesAvailable.at(m_shortTimeCombo->currentIndex());
    m_format.longTimeFormat = regionFormat.longTimesAvailable.at(m_longTimeCombo->currentIndex());
    m_format.currencyFormat = m_currencyValueLabel->text();
    m_format.numberFormat = m_numberValueLabel->text();
    m_format.paperFormat = m_paperValueLabel->text();

    Q_EMIT customFormatSaved(m_format);
}
