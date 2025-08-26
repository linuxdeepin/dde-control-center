//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimeworker.h"
#include "datetimedbusproxy.h"
#include "regionproxy.h"
#include <QDebug>

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QLoggingCategory>

#include <dconfig.h>
#include <qvariant.h>

Q_LOGGING_CATEGORY(DdcDateTimeWorkder, "dcc-datetime-worker")

DatetimeWorker::DatetimeWorker(DatetimeModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_timedateInter(new DatetimeDBusProxy(this))
    , m_regionInter(new RegionProxy(this))
    , m_config(DTK_CORE_NAMESPACE::DConfig::createGeneric("org.deepin.region-format", QString(), this))
{
    QMetaObject::invokeMethod(this, "activate", Qt::QueuedConnection);
#ifndef DCC_DISABLE_TIMEZONE
    connect(m_timedateInter, &DatetimeDBusProxy::UserTimezonesChanged, this, &DatetimeWorker::onTimezoneListChanged);
    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, m_model, &DatetimeModel::setSystemTimeZoneId);
#endif
    connect(m_timedateInter, &DatetimeDBusProxy::NTPChanged, m_model, &DatetimeModel::setNTP);
    connect(m_timedateInter, &DatetimeDBusProxy::Use24HourFormatChanged, m_model, &DatetimeModel::set24HourFormat);

    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, this, [=](const QString &value) {
        auto tzinfo = GetZoneInfo(value);
        if (tzinfo.getZoneName().length() == 0) {
            tzinfo = GetZoneInfo(QTimeZone::systemTimeZoneId());
        }
        m_model->setCurrentUseTimeZone(tzinfo);
        QList<ZoneInfo> zones = m_model->userTimeZones();
        // Remove the timezone from user list if it becomes the system timezone
        for (const ZoneInfo &zone : zones) {
            if (zone.getZoneName() == value) {
                m_model->removeUserTimeZone(zone);
                onTimezoneListChanged(m_timedateInter->userTimezones());
                break;
            }
        }
    });

    connect(m_timedateInter, &DatetimeDBusProxy::NTPServerChanged, m_model, &DatetimeModel::setNtpServerAddress);
    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, m_model, &DatetimeModel::setTimeZoneInfo);
    connect(m_timedateInter, &DatetimeDBusProxy::WeekdayFormatChanged, m_model, &DatetimeModel::weekdayFormatChanged);

    connect(m_timedateInter, &DatetimeDBusProxy::CurrencySymbolChanged, m_model, [this](const QString &symbol) {
        Q_EMIT m_model->symbolChanged(DatetimeModel::CurrencySymbol, symbol);
    });
    connect(m_timedateInter, &DatetimeDBusProxy::NegativeCurrencyFormatChanged, m_model, [this](const QString &symbol) {
        Q_EMIT m_model->symbolChanged(DatetimeModel::CurrencySymbol, symbol);
    });
    connect(m_timedateInter, &DatetimeDBusProxy::PositiveCurrencyFormatChanged, m_model, [this](const QString &symbol) {
        Q_EMIT m_model->symbolChanged(DatetimeModel::CurrencySymbol, symbol);
    });
    connect(m_timedateInter, &DatetimeDBusProxy::DecimalSymbolChanged, m_model, [this](const QString &symbol){
        Q_EMIT m_model->symbolChanged(DatetimeModel::DecimalSymbol, symbol);
    });
    connect(m_timedateInter, &DatetimeDBusProxy::DigitGroupingChanged, m_model, [this](const QString &symbol){
        Q_EMIT m_model->symbolChanged(DatetimeModel::DigitGrouping, symbol);
    });
    connect(m_timedateInter, &DatetimeDBusProxy::DigitGroupingSymbolChanged, m_model, [this](const QString &symbol){
        m_model->setDigitGroupingSymbol(symbol);
        Q_EMIT m_model->symbolChanged(DatetimeModel::DigitGroupingSymbol, symbol);
    });

    m_model->setCurrentTimeZone(GetZoneInfo(QTimeZone::systemTimeZoneId()));
    m_model->setCurrentUseTimeZone(GetZoneInfo(m_timedateInter->timezone()));
    m_model->set24HourFormat(m_timedateInter->use24HourFormat());

    refreshNtpServerList();
    m_model->setNtpServerAddress(m_timedateInter->nTPServer());
    m_model->setTimeZoneInfo(m_timedateInter->timezone());
    m_model->setNTP(m_timedateInter->nTP());
    m_model->setDigitGroupingSymbol(m_timedateInter->digitGroupingSymbol());

    initRegionFormatData();
}

DatetimeWorker::~DatetimeWorker()
{
}

void DatetimeWorker::activate()
{
    if (!m_regionInter->isActive()) {
        m_regionInter->active();
        m_model->setCountries(m_regionInter->countries());
        m_model->setRegions(m_regionInter->regions());
    }
    m_model->setNTP(m_timedateInter->nTP());
#ifndef DCC_DISABLE_TIMEZONE
    m_model->setSystemTimeZoneId(m_timedateInter->timezone());
    onTimezoneListChanged(m_timedateInter->userTimezones());
#endif
}

void DatetimeWorker::deactivate()
{
}

DatetimeModel *DatetimeWorker::model() { return m_model; }

void DatetimeWorker::setNTP(bool ntp)
{
    Q_EMIT requestSetAutoHide(false);
    m_timedateInter->SetNTP(ntp, this, SLOT(setAutoHide()), SLOT(setNTPError()));
}
void DatetimeWorker::setAutoHide()
{
    Q_EMIT requestSetAutoHide(true);
}
void DatetimeWorker::setNTPError()
{
    Q_EMIT m_model->ntpChanged(m_model->nTP());
    setAutoHide();
}

void DatetimeWorker::setDatetime(const QDateTime &datetime)
{
    Q_EMIT requestSetAutoHide(false);
    qCDebug(DdcDateTimeWorkder) << "start setDatetime";
    m_setDatetime = new QDateTime(datetime);
    m_timedateInter->SetNTP(false, this, SLOT(setDatetimeStart()), SLOT(setAutoHide()));
}
void DatetimeWorker::setDatetimeStart()
{
    if (m_setDatetime) {
        qCDebug(DdcDateTimeWorkder) << "set ntp success, m_timedateInter->SetDate";
        m_timedateInter->SetDate(*m_setDatetime, this, SLOT(setDateFinished()));
        delete m_setDatetime;
        m_setDatetime = nullptr;
    }
    setAutoHide();
}
void DatetimeWorker::setDateFinished()
{
    Q_EMIT m_model->systemTimeChanged();
}
void DatetimeWorker::set24HourType(bool state)
{
    m_timedateInter->setUse24HourFormat(state);
}

#ifndef DCC_DISABLE_TIMEZONE
void DatetimeWorker::setTimezone(const QString &timezone)
{
    m_timedateInter->SetTimezone(timezone, tr("Authentication is required to set the system timezone"));
}

void DatetimeWorker::removeUserTimeZone(const QString &zone)
{
    m_timedateInter->DeleteUserTimezone(zone);
}

void DatetimeWorker::addUserTimeZone(const QString &zone)
{
    m_timedateInter->AddUserTimezone(zone);
}

void DatetimeWorker::setNtpServer(QString server)
{
    qInfo() << "Try set server : " << server;

    if (server == m_timedateInter->nTPServer())
        return;
    m_timedateInter->SetNTPServer(server, tr("Authentication is required to change NTP server"), this, SLOT(SetNTPServerFinished()), SLOT(SetNTPServerError()));
}

int DatetimeWorker::weekdayFormat()
{
    return m_timedateInter->weekdayFormat();
}

void DatetimeWorker::SetNTPServerFinished()
{
    qInfo() << "set server success.";
    Q_EMIT m_model->NTPServerChanged(m_timedateInter->nTPServer());
}
void DatetimeWorker::SetNTPServerError()
{
    qInfo() << "Not set server success.";
    Q_EMIT m_model->NTPServerNotChanged(m_timedateInter->nTPServer());
}

void DatetimeWorker::setWeekdayFormat(int type)
{
    m_timedateInter->setWeekdayFormat(type);
}

void DatetimeWorker::setShortDateFormat(int type)
{
    m_timedateInter->setShortDateFormat(type);
}

void DatetimeWorker::setLongDateFormat(int type)
{
    m_timedateInter->setLongDateFormat(type);
}

void DatetimeWorker::setLongTimeFormat(int type)
{
    m_timedateInter->setLongTimeFormat(type);
}

void DatetimeWorker::setShortTimeFormat(int type)
{
    m_timedateInter->setShortTimeFormat(type);
}

void DatetimeWorker::setWeekStartDayFormat(int type)
{
    m_timedateInter->setWeekBegins(type);
}

void DatetimeWorker::onTimezoneListChanged(const QStringList &timezones)
{
    QList<ZoneInfo> zones = m_model->userTimeZones();
    QStringList zonesName;
    for (const ZoneInfo &zone : zones) {
        zonesName.append(zone.getZoneName());
    }
    for (const QString &zone : timezones) {
        zonesName.removeOne(zone);
        m_timedateInter->GetZoneInfo(zone, this, SLOT(getZoneInfoFinished(ZoneInfo)));
    }
    for (const ZoneInfo &zone : zones) {
        if (zonesName.contains(zone.getZoneName())) {
            m_model->removeUserTimeZone(zone);
        }
    }
}

void DatetimeWorker::getZoneInfoFinished(ZoneInfo zoneInfo)
{
    m_model->addUserTimeZone(zoneInfo);
}

QString DatetimeWorker::decimalSymbol()
{
    return m_timedateInter->decimalSymbol();
}

void DatetimeWorker::setDecimalSymbol(const QString &value)
{
    return m_timedateInter->setDecimalSymbol(value);
}

QString DatetimeWorker::digitGrouping()
{
    return m_timedateInter->digitGrouping();
}

void DatetimeWorker::setDigitGrouping(const QString &value)
{
    return m_timedateInter->setDigitGrouping(value);
}

QString DatetimeWorker::digitGroupingSymbol()
{
    return m_timedateInter->digitGroupingSymbol();
}

void DatetimeWorker::setDigitGroupingSymbol(const QString &value)
{
    return m_timedateInter->setDigitGroupingSymbol(value);
}

QString DatetimeWorker::currencySymbol()
{
    return m_timedateInter->currencySymbol();
}

void DatetimeWorker::setCurrencySymbol(const QString &value)
{
    return m_timedateInter->setCurrencySymbol(value);
}

QString DatetimeWorker::negativeCurrencyFormat()
{
    return m_timedateInter->negativeCurrencyFormat();
}

void DatetimeWorker::setNegativeCurrencyFormat(const QString &value)
{
    return m_timedateInter->setNegativeCurrencyFormat(value);
}

QString DatetimeWorker::positiveCurrencyFormat()
{
    return m_timedateInter->positiveCurrencyFormat();
}

void DatetimeWorker::setPositiveCurrencyFormat(const QString &value)
{
    return m_timedateInter->setPositiveCurrencyFormat(value);
}
#endif

void DatetimeWorker::refreshNtpServerList()
{
    m_timedateInter->GetSampleNTPServers(this, SLOT(getSampleNTPServersFinished(const QStringList &)));
}

void DatetimeWorker::getSampleNTPServersFinished(const QStringList &serverList)
{
    m_model->setNTPServerList(serverList);
}

ZoneInfo DatetimeWorker::GetZoneInfo(const QString &zoneId)
{
    return m_timedateInter->GetZoneInfo(zoneId);
}

void DatetimeWorker::initRegionFormatData()
{
    if (!m_config->isValid())
        return;

    if (m_config->isDefaultValue(country_key)) {
        setConfigValue(country_key, m_regionInter->systemCountry());
        m_model->setCountry(m_regionInter->systemCountry());
    } else {
        m_model->setCountry(m_config->value(country_key).toString());
    }
    if (m_config->isDefaultValue(languageRegion_key) || m_config->value(languageRegion_key).toString().isEmpty()) {
        setConfigValue(languageRegion_key, m_regionInter->langCountry());
        m_model->setLangRegion(m_regionInter->langCountry());
    } else {
        m_model->setLangRegion(m_config->value(languageRegion_key).toString());
    }
    if (m_config->isDefaultValue(localeName_key)) {
        setConfigValue(localeName_key, QLocale::system().name());
        m_model->setLocaleName(QLocale::system().name());
    } else {
        m_model->setLocaleName(m_config->value(localeName_key).toString());
    }
    if (m_config->isDefaultValue(firstDayOfWeek_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(firstDayOfWeek_key, m_regionInter->regionFormat(locale).firstDayOfWeekFormat);
        m_model->setFirstDayOfWeek(m_regionInter->regionFormat(locale).firstDayOfWeekFormat);
    } else {
        m_model->setFirstDayOfWeek(m_config->value(firstDayOfWeek_key).toInt());
    }
    if (m_config->isDefaultValue(shortDateFormat_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(shortDateFormat_key, m_regionInter->regionFormat(locale).shortDateFormat);
        m_model->setShortDateFormat(m_regionInter->regionFormat(locale).shortDateFormat);
    } else {
        m_model->setShortDateFormat(m_config->value(shortDateFormat_key).toString());
    }
    if (m_config->isDefaultValue(longDateFormat_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(longDateFormat_key, m_regionInter->regionFormat(locale).longDateFormat);
        m_model->setLongDateFormat(m_regionInter->regionFormat(locale).longDateFormat);
    } else {
        m_model->setLongDateFormat(m_config->value(longDateFormat_key).toString());
    }
    if (m_config->isDefaultValue(shortTimeFormat_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(shortTimeFormat_key, m_regionInter->regionFormat(locale).shortTimeFormat);
        m_model->setShortTimeFormat(m_regionInter->regionFormat(locale).shortTimeFormat);
    } else {
        m_model->setShortTimeFormat(m_config->value(shortTimeFormat_key).toString());
    }
    if (m_config->isDefaultValue(longTimeFormat_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(longTimeFormat_key, m_regionInter->regionFormat(locale).longTimeFormat);
        m_model->setLongTimeFormat(m_regionInter->regionFormat(locale).longTimeFormat);
    } else {
        m_model->setLongTimeFormat(m_config->value(longTimeFormat_key).toString());
    }
    if (m_config->isDefaultValue(currencyFormat_key)) {
        QLocale locale(QLocale::system().name());
        const auto newFormat = m_regionInter->regionFormat(locale).currencyFormat;
        setConfigValue(currencyFormat_key, newFormat);
        m_model->setCurrencyFormat(newFormat);

        const auto oldFormat = currencySymbol();
        auto posFmt = positiveCurrencyFormat();
        posFmt.replace(oldFormat, newFormat);
        auto negFmt = negativeCurrencyFormat();
        negFmt.replace(oldFormat, newFormat);
        QMetaObject::invokeMethod(this, "setCurrencySymbol", Qt::QueuedConnection, newFormat);
        QMetaObject::invokeMethod(this, "setPositiveCurrencyFormat", Qt::QueuedConnection, posFmt);
        QMetaObject::invokeMethod(this, "setNegativeCurrencyFormat", Qt::QueuedConnection, negFmt);
    } else {
        m_model->setCurrencyFormat(m_config->value(currencyFormat_key).toString());
    }
    if (m_config->isDefaultValue(numberFormat_key)) {
        QLocale locale(QLocale::system().name());
        setConfigValue(numberFormat_key, m_regionInter->regionFormat(locale).numberFormat);
        m_model->setNumberFormat(m_regionInter->regionFormat(locale).numberFormat);
    } else {
        m_model->setNumberFormat(m_config->value(numberFormat_key).toString());
    }
    if (m_config->isDefaultValue(paperFormat_key)) {
        QLocale locale(QLocale::system().name());
        m_model->setPaperFormat(m_regionInter->regionFormat(locale).paperFormat);
        setConfigValue(paperFormat_key, m_regionInter->regionFormat(locale).paperFormat);
    } else {
        m_model->setPaperFormat(m_config->value(paperFormat_key).toString());
    }

    RegionFormat regionFormat;
    regionFormat.firstDayOfWeekFormat = m_model->firstDayOfWeekFormat();
    regionFormat.shortDateFormat = m_model->shortDateFormat();
    regionFormat.longDateFormat = m_model->longDateFormat();
    regionFormat.shortTimeFormat = m_model->shortTimeFormat();
    regionFormat.longTimeFormat = m_model->longTimeFormat();
    regionFormat.paperFormat = m_model->paperFormat();
    regionFormat.currencyFormat = m_model->currencyFormat();
    regionFormat.numberFormat = m_model->numberFormat();
    m_model->setRegionFormat(regionFormat);

    connect(m_config, &DTK_CORE_NAMESPACE::DConfig::valueChanged, this, [this] (const QString &key) {
        if (key == country_key) {
            m_model->setCountry(m_config->value(key).toString());
        } else if (key == languageRegion_key) {
            m_model->setLangRegion(m_config->value(key).toString());
        } else if (key == localeName_key) {
            m_model->setLocaleName(m_config->value(key).toString());
        } else if (key == firstDayOfWeek_key) {
            m_model->setFirstDayOfWeek(m_config->value(key).toInt());
        } else if (key == shortDateFormat_key) {
            m_model->setShortDateFormat(m_config->value(key).toString());
        } else if (key == longDateFormat_key) {
            m_model->setLongDateFormat(m_config->value(key).toString());
        } else if (key == shortTimeFormat_key) {
            m_model->setShortTimeFormat(m_config->value(key).toString());
        } else if (key == longTimeFormat_key) {
            m_model->setLongTimeFormat(m_config->value(key).toString());
        } else if (key == currencyFormat_key) {
            m_model->setCurrencyFormat(m_config->value(key).toString());
        } else if (key == numberFormat_key) {
            m_model->setNumberFormat(m_config->value(key).toString());
        } else if (key == paperFormat_key) {
            m_model->setPaperFormat(m_config->value(key).toString());
        }
    });
}

std::optional<QStringList> DatetimeWorker::getSupportedLocale()
{
    if (m_supportedLocaleList.has_value()) {
        return m_supportedLocaleList;
    }
    static QString LOCALE_LIST_FILE = QStringLiteral(LOCALE_I18N_PATH);
    QFile file(LOCALE_LIST_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;
    QStringList localelist;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList out = line.split(" ");
        localelist.push_back(out[0]);
    }
    m_supportedLocaleList = localelist;
    return m_supportedLocaleList;
}

std::optional<LocaleList> DatetimeWorker::getAllLocale()
{
    return m_timedateInter->getLocaleListMap();
}

std::optional<QString> DatetimeWorker::getLocaleRegion()
{
    return m_timedateInter->getLocaleRegion();
}

void DatetimeWorker::setLocaleRegion(const QString &locale)
{
    m_timedateInter->setLocaleRegion(locale);
}

void DatetimeWorker::setConfigValue(const QString &key, const QVariant &value)
{
    m_config->setValue(key, value);
}

bool DatetimeWorker::genLocale(const QString &localeName)
{
    static QString localeConfPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "locale.conf";

    QSettings settings(localeConfPath, QSettings::IniFormat);

    auto supportedLocaleListRes = getSupportedLocale();
    if (!supportedLocaleListRes.has_value()) {
        return false;
    }

    QStringList supportedLocaleList = supportedLocaleListRes.value();
    QString localeSet;
    if (QString otherLocaleName = localeName + ".UTF-8";
        supportedLocaleList.contains(otherLocaleName)) {
        localeSet = otherLocaleName;
    } else if (supportedLocaleList.contains(localeName)) {
        localeSet = localeName;
    } else {
        return false;
    }
    settings.setValue("LC_NUMERIC", localeSet);
    settings.setValue("LC_MONETARY", localeSet);
    settings.setValue("LC_TIME", localeSet);
    settings.setValue("LC_PAPER", localeSet);
    settings.setValue("LC_NAME", localeSet);
    settings.setValue("LC_ADDRESS", localeSet);
    settings.setValue("LC_TELEPHONE", localeSet);
    settings.setValue("LC_MEASUREMENT", localeSet);

    m_timedateInter->GenLocale(localeSet);

    return true;
}
