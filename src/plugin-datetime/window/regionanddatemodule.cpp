// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "regionanddatemodule.h"

#include "datetimemodel.h"
#include "datetimeworker.h"
#include "itemmodule.h"
#include "widgets/comboxwidget.h"
#include "widgets/region_setting_dialog.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/widgetmodule.h"

#include <DFontSizeManager>
#include <DIconButton>
#include <DTipLabel>

#include <QDateTime>
#include <QGridLayout>
#include <QPushButton>

using namespace DCC_NAMESPACE;
using Dtk::Widget::DFontSizeManager;
using Dtk::Widget::DIconButton;
using Dtk::Widget::DTipLabel;

RegionAndFormatModule::RegionAndFormatModule(DatetimeModel *model,
                                             DatetimeWorker *work,
                                             QObject *parent)
    : PageModule("regionanddate", tr("Region and Format"), parent)
    , m_model(model)
    , m_work(work)
    , m_fotmatWeek({ tr("Monday"), tr("monday") })
    , m_fotmatLongDate(
              { tr("April 5, 2020"), tr("April 5, 2020, Sunday"), tr("Sunday, April 5, 2020") })
    , m_fotmatShortDate({ tr("2020/4/5"),
                          tr("2020-4-5"),
                          tr("2020.4.5"),
                          tr("2020/04/05"),
                          tr("2020-04-05"),
                          tr("2020.04.05"),
                          tr("20/4/5"),
                          tr("20-4-5"),
                          tr("20.4.5") })
    , m_fotmatLongTime({ tr("9:40:07"), tr("09:40:07") })
    , m_fotmatShortTime({ tr("9:40"), tr("09:40") })
    , m_weekStartWithDay({ tr("Monday"),
                           tr("Tuesday"),
                           tr("Wednesday"),
                           tr("Thursday"),
                           tr("Friday"),
                           tr("Saturday"),
                           tr("Sunday") })
{
    // TODO hide region's view
    if (false) {
    appendChild(new ItemModule("regionalsetting", tr("Regional Setting")));
    appendChild(new ItemModule(
            "RegionSelectButton",
            tr(""),
            [this](ModuleObject *) {
                QLocale locale;
                QString localeName = m_work->getLocaleRegion().value_or(locale.name());
                auto button = new RegionSettingBtn(localeName);
                connect(button, &RegionSettingBtn::clicked, this, [this] {
                    auto localeList = m_work->getAllLocale();
                    if (!localeList.has_value()) {
                        return;
                    }
                    auto dialog = RegionDialog(localeList.value());
                    if (dialog.exec() == QDialog::Accepted) {
                        // TODO: set the locale
                        // Because locale need to be logout and update, so after setted, it will not
                        // update the value So just set it will be fine
                        m_work->setLocaleRegion(dialog.selectedValue()->first);
                    }
                });
                return button;
            },
            false));
    appendChild(
            new WidgetModule<DTipLabel>("RegionTip", tr(""), [](DTipLabel *internalUpdateLabel) {
                internalUpdateLabel->setWordWrap(true);
                internalUpdateLabel->setAlignment(Qt::AlignLeft);
                internalUpdateLabel->setContentsMargins(10, 0, 10, 0);
                internalUpdateLabel->setText(
                        tr("* The setting of region will influence the formats of date, time, "
                           "number and some other "
                           "formats, it will be enabled on the next time of login"));
            }));
    appendChild(new ItemModule(
            "LocaleShow",
            tr("Locale Show"),
            [](ModuleObject *) {
                return new FormatShowGrid;
            },
            false));
    }

    appendChild(new ItemModule("timetitle", tr("Time")));
    appendChild(new WidgetModule<SettingsGroup>("Time",
                                                tr("time"),
                                                this,
                                                &RegionAndFormatModule::initTimeFotmat));
    appendChild(new ItemModule("dateTitle", tr("Date")));
    appendChild(new WidgetModule<SettingsGroup>("Date",
                                                tr("Date"),
                                                this,
                                                &RegionAndFormatModule::initDateFotmat));
}

void RegionAndFormatModule::initTimeFotmat(DCC_NAMESPACE::SettingsGroup *timeGrp)
{
    timeGrp->setBackgroundStyle(SettingsGroup::GroupBackground);

    SwitchWidget *hourTypeSwitch = new SwitchWidget;
    hourTypeSwitch->setTitle(tr("24-hour Time"));
    hourTypeSwitch->setChecked(m_model->get24HourFormat());
    // true : 24 hour type  ,  false : 12 hour type
    connect(hourTypeSwitch, &SwitchWidget::checkedChanged, m_work, &DatetimeWorker::set24HourType);
    connect(m_model, &DatetimeModel::hourTypeChanged, hourTypeSwitch, &SwitchWidget::setChecked);
    timeGrp->appendItem(hourTypeSwitch);

    ComboxWidget *shortimeCbx = new ComboxWidget(tr("Short Time")); // 短时间
    shortimeCbx->comboBox()->addItems(m_fotmatShortTime);
    shortimeCbx->comboBox()->setCurrentIndex(m_model->shorTimeFormat());
    connect(m_model,
            &DatetimeModel::shorTimeFormatChanged,
            shortimeCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(shortimeCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setShortTimeFormat);

    ComboxWidget *longtimeCbx = new ComboxWidget(tr("Long Time")); // 长时间
    longtimeCbx->comboBox()->addItems(m_fotmatLongTime);
    longtimeCbx->comboBox()->setCurrentIndex(m_model->longTimeFormat());
    connect(m_model,
            &DatetimeModel::longTimeFormatChanged,
            longtimeCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(longtimeCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setLongTimeFormat);
    timeGrp->appendItem(shortimeCbx);
    timeGrp->appendItem(longtimeCbx);
}

void RegionAndFormatModule::initDateFotmat(DCC_NAMESPACE::SettingsGroup *timeGrp)
{
    timeGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    ComboxWidget *weekCbx = new ComboxWidget(tr("Weeks")); // 星期
    weekCbx->setVisible(QLocale::system().name().contains("zh"));
    weekCbx->comboBox()->addItems(m_fotmatWeek);
    weekCbx->comboBox()->setCurrentIndex(m_model->weekdayFormatType());
    connect(m_model,
            &DatetimeModel::weekdayFormatTypeChanged,
            weekCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(weekCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setWeekdayFormat);

    ComboxWidget *weekStartDayCbx = new ComboxWidget(tr("First Day of Week")); // 一周首日
    weekStartDayCbx->comboBox()->addItems(m_weekStartWithDay);
    weekStartDayCbx->comboBox()->setCurrentIndex(m_model->weekStartDayFormat());
    connect(m_model,
            &DatetimeModel::weekStartDayFormatChanged,
            weekStartDayCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(weekStartDayCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setWeekStartDayFormat);

    ComboxWidget *shortDateCbx = new ComboxWidget(tr("Short Date")); // 短日期
    shortDateCbx->comboBox()->addItems(m_fotmatShortDate);
    shortDateCbx->comboBox()->setCurrentIndex(m_model->shortDateFormat());
    connect(m_model,
            &DatetimeModel::shortDateFormatChanged,
            shortDateCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(shortDateCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setShortDateFormat);

    ComboxWidget *longdateCbx = new ComboxWidget(tr("Long Date")); // 长日期
    longdateCbx->comboBox()->addItems(m_fotmatLongDate);
    longdateCbx->comboBox()->setCurrentIndex(m_model->longDateFormat());
    connect(m_model,
            &DatetimeModel::longDateFormatChanged,
            longdateCbx->comboBox(),
            &QComboBox::setCurrentIndex);
    connect(longdateCbx->comboBox(),
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            m_work,
            &DatetimeWorker::setLongDateFormat);

    timeGrp->appendItem(weekCbx);
    timeGrp->appendItem(weekStartDayCbx);
    timeGrp->appendItem(shortDateCbx);
    timeGrp->appendItem(longdateCbx);
}

InforShowUnit::InforShowUnit(const QString &title, const QString &info, QWidget *parent)
    : QWidget(parent)
    , m_info(new QLabel(info, this))
{
    auto center = new QVBoxLayout(this);
    auto titleLabel = new QLabel(title, this);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T6, QFont::DemiBold);
    center->addWidget(titleLabel);
    center->addWidget(m_info);
}

void InforShowUnit::setInfo(const QString &info)
{
    m_info->setText(info);
}

FormatShowGrid::FormatShowGrid(QWidget *parent)
    : SettingsItem(parent)
{
    QLocale currentLocale;
    QString currentDate = currentLocale.toString(QDate::currentDate());
    QString currentTime = currentLocale.toString(QTime::currentTime());
    QString currentDateAndTime = currentLocale.toString(QDateTime::currentDateTime());
    QString numberToShow = currentLocale.toString(123456789);
    QString currency = currentLocale.currencySymbol();

    QGridLayout *center = new QGridLayout(this);
    center->addWidget(new InforShowUnit(tr("Date"), currentDate), 0, 0, 1, 1);
    center->addWidget(new InforShowUnit(tr("Time"), currentTime), 0, 1, 1, 1);
    center->addWidget(new InforShowUnit(tr("Date and Time"), currentDateAndTime), 0, 2, 1, 2);

    center->addWidget(new InforShowUnit(tr("Number"), numberToShow), 1, 0, 1, 1);
    center->addWidget(new InforShowUnit(tr("Currency"), currency), 1, 1, 1, 1);
    addBackground();
}

RegionSettingBtn::RegionSettingBtn(const QString &region, QWidget *parent)
    : SettingsItem(parent)
    , m_regionLabel(new QLabel(region, this))
{
    QHBoxLayout *center = new QHBoxLayout(this);
    center->setContentsMargins(12, 5, 12, 5);

    center->addWidget(m_regionLabel);
    center->addStretch();

    DIconButton *right = new DIconButton;
    right->setIcon(QIcon::fromTheme("go-next"));
    connect(right, &QPushButton::clicked, this, &RegionSettingBtn::clicked);
    center->addWidget(right);

    addBackground();
}
