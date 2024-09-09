//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimemodel.h"
#include "dccfactory.h"
#include "timezoneMap/timezone_map_util.h"
#include "datetimeworker.h"
#include "zoneinfomodel.h"

#include <unicode/locid.h>
#include <unicode/unistr.h>

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>
#include <QCoreApplication>
#include <QStringListModel>
static installer::ZoneInfoList g_totalZones;

static QString getDescription(const ZoneInfo &zoneInfo)
{
    const QDateTime localTime(QDateTime::currentDateTime());
    const double timeDelta = (zoneInfo.getUTCOffset() - localTime.offsetFromUtc()) / 3600.0;
    QString dateLiteral;
    if (localTime.time().hour() + timeDelta >= 24) {
        dateLiteral = DatetimeModel::tr("Tomorrow");
    } else if (localTime.time().hour() + timeDelta <= 0) {
        dateLiteral = DatetimeModel::tr("Yesterday");
    } else {
        dateLiteral = DatetimeModel::tr("Today");
    }

    int decimalNumber = 1;
    //小时取余,再取分钟,将15分钟的双倍只显示一位小数,其他的都显示两位小数
    switch ((zoneInfo.getUTCOffset() - localTime.offsetFromUtc()) % 3600 / 60 / 15) {
    case -1:
    case -3:
    case 1:
    case 3:
        decimalNumber = 2;
        break;
    default:
        decimalNumber = 1;
        break;
    }

    QString description;
    if (timeDelta > 0) {
        description = DatetimeModel::tr("%1 hours earlier than local").arg(QString::number(timeDelta, 'f', decimalNumber));
    } else {
        description = DatetimeModel::tr("%1 hours later than local").arg(QString::number(-timeDelta, 'f', decimalNumber));
    }

    return description;
}

static QString getUtcOffsetText(int utcOffset)
{
    QString gmData;
    int utcOff = utcOffset / 3600;
    if (utcOff >= 0) {
        gmData = QString("(UTC+%1:%2)").arg(utcOff, 2, 10, QLatin1Char('0')).arg(utcOffset % 3600 / 60, 2, 10, QLatin1Char('0'));
    } else {
        gmData = QString("(UTC%1:%2)").arg(utcOff, 3, 10, QLatin1Char('0')).arg(utcOffset % 3600 / 60, 2, 10, QLatin1Char('0'));
    }

    return gmData;
}

static QString getDisplayText(const ZoneInfo &zoneInfo)
{
    QString gmData = getUtcOffsetText(zoneInfo.getUTCOffset());
    QString cityName = zoneInfo.getZoneCity().isEmpty() ? zoneInfo.getZoneName() : zoneInfo.getZoneCity();

    return QString("%1 %2").arg(cityName).arg(gmData);
}

static QStringList timeZoneList(const installer::ZoneInfoList &zoneInfoList, QMap<QString, QString> &cache)
{
    using namespace installer;
    if (g_totalZones.empty())
        g_totalZones =  GetZoneInfoList();

    const QString locale = QLocale::system().name();
    QStringList timezoneList;
    for (const auto& info : zoneInfoList) {
        auto localzone = GetLocalTimezoneName(info.timezone, locale);

        if (!cache.contains(localzone)) {
            // "上海": "Asia/Shanghai"
            cache[localzone] = info.timezone;
        }

        timezoneList << localzone;
    }

    return timezoneList;
}

DatetimeModel::DatetimeModel(QObject *parent)
    : QObject(parent)
    , m_ntp(true)
    , m_bUse24HourType(true)
    , m_work(new DatetimeWorker(this, this))
{
    connect(this, &DatetimeModel::ntpChanged, m_work, &DatetimeWorker::setNTP);
    connect(this, &DatetimeModel::hourTypeChanged, m_work, &DatetimeWorker::set24HourType);
    connect(this, &DatetimeModel::NTPServerChanged, m_work, &DatetimeWorker::setNtpServer);
    // 设置ntp地址失败回退到之前的地址
    connect(this, &DatetimeModel::NTPServerNotChanged, this, &DatetimeModel::setNtpServerAddress);

    connect(this, &DatetimeModel::userTimeZoneAdded, m_work, [this](const ZoneInfo &zone){
        m_work->addUserTimeZone(zone.getZoneName());
    });
    connect(this, &DatetimeModel::userTimeZoneRemoved, m_work, &DatetimeWorker::removeUserTimeZone);
    // set timezone
    connect(this, &DatetimeModel::timeZoneChanged, m_work, &DatetimeWorker::setTimezone);
}

void DatetimeModel::setNTP(bool ntp)
{
    if (m_ntp != ntp) {
        m_ntp = ntp;
        Q_EMIT ntpChanged(ntp);
    }
}

void DatetimeModel::set24HourFormat(bool state)
{
    if (m_bUse24HourType != state) {
        m_bUse24HourType = state;
        Q_EMIT hourTypeChanged(state);
    }
}

void DatetimeModel::setDateTime(const QDateTime &dateTime)
{
    if (m_work)
        m_work->setDatetime(dateTime);
}

QStringList DatetimeModel::zones(int x, int y, int map_width, int map_height)
{
    using namespace installer;
    if (g_totalZones.empty())
        g_totalZones =  GetZoneInfoList();

    const double kDistanceThreshold = 64.0;
    auto zonelist = GetNearestZones(g_totalZones, kDistanceThreshold, x, y, map_width, map_height);

    return timeZoneList(zonelist, m_timezoneCache);
}

QPoint DatetimeModel::zonePosition(const QString &timezone, int map_width, int map_height)
{
    using namespace installer;
    if (g_totalZones.empty())
        g_totalZones =  GetZoneInfoList();

    auto enZone = m_timezoneCache.value(timezone, timezone);

    int index = GetZoneInfoByZone(g_totalZones, enZone);
    if (index < 0)
        return QPoint();

    auto currentZone = g_totalZones.at(index);

    const int x = int(ConvertLongitudeToX(currentZone.longitude) * map_width);
    const int y = int(ConvertLatitudeToY(currentZone.latitude) * map_height);
    return QPoint(x, y);
}

QStringList DatetimeModel::zoneIdList()
{
    using namespace installer;
    if (g_totalZones.empty())
        g_totalZones =  GetZoneInfoList();

    QStringList list;
    for (const auto& info : g_totalZones) {
        list << info.timezone;
    }

    return list;
}

QString DatetimeModel::zoneDisplayName(const QString &zoneName)
{
    if (m_work) {
        auto zoneInfo = m_work->GetZoneInfo(zoneName);
        QString utcOffsetText = getUtcOffsetText(zoneInfo.getUTCOffset());
        QString cityName = zoneInfo.getZoneCity().isEmpty() ? zoneInfo.getZoneName() : zoneInfo.getZoneCity();
        return QString("%1 %2").arg(utcOffsetText).arg(cityName);
    }
    return QString();
}

QSortFilterProxyModel *DatetimeModel::searchModel()
{
    if (m_searchModel)
        return m_searchModel;

    m_searchModel = new QSortFilterProxyModel(this);

    auto sourceModel = new dccV25::ZoneInfoModel(this);
    m_searchModel->setSourceModel(sourceModel);
    m_searchModel->setFilterRole(dccV25::ZoneInfoModel::SearchTextRole);
    m_searchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_searchModel;
}

QStringList DatetimeModel::languagesAndRegions()
{
    QStringList langAndRegions;
    auto localeSystem = QLocale::system();
    auto systemLocale = icu::Locale(localeSystem.name().toStdString().data());
    for (auto locale : m_regions) {
        auto IcuLocale = icu::Locale(locale.name().toStdString().data());
        auto localeHex = icu::UnicodeString(locale.name().toStdString().data());
        std::string displayLanguageIcu;
        IcuLocale.getDisplayLanguage(systemLocale, localeHex).toUTF8String(displayLanguageIcu);
        std::string displayCountryIcu;
        IcuLocale.getDisplayCountry(systemLocale, localeHex).toUTF8String(displayCountryIcu);
        QString displaylanguage = QString::fromStdString(displayLanguageIcu);
        QString displayCountry = QString::fromStdString(displayCountryIcu);
        QString langRegion = m_regions.key(locale);
        QString langCountry = QString("%1(%2)").arg(displaylanguage).arg(displayCountry);
        QStringList langRegions = langRegion.split(":");
        if (langRegions.size() >= 2
            && (langRegions[0] == "Traditional Chinese"
                || langRegions[0] == "Simplified Chinese"
                || langRegions[1] == QLocale::countryToString(QLocale::HongKong)
                || langRegions[1] == QLocale::countryToString(QLocale::Macau)
                || langRegions[1] == QLocale::countryToString(QLocale::Taiwan))) {
            langCountry =
                    QString("%1(%2)")
                            .arg(QCoreApplication::translate("dcc::datetime::Language",
                                                             langRegions.at(0).toUtf8().data()))
                            .arg(QCoreApplication::translate("dcc::datetime::Country",
                                                             langRegions.at(1).toUtf8().data()));
        }
        langAndRegions << langCountry;
    }

    return langAndRegions;
}

void DatetimeModel::addUserTimeZoneByName(const QString &zoneName)
{
    if (!m_timezoneCache.contains(zoneName)) {
        qWarning() << "timezone cache not contain.." << zoneName;
        return;
    }
    QString zoneId = m_timezoneCache.value(zoneName);

    if (m_userZoneIds.contains(zoneId)) {
        qWarning() << "user timezone already existed";
        return;
    }

    if (m_work)
        m_work->addUserTimeZone(zoneId);
}
void DatetimeModel::removeUserTimeZoneByName(const QString &name)
{
    // displayText list
    auto zonelist = userTimeZoneText(0);
    int index = zonelist.indexOf(name);
    if (index < 0) {
      qWarning() << name << "Not found in User TimeZones";
        return;
    }

    removeUserTimeZone(m_userTimeZones.value(index));
}

#ifndef DCC_DISABLE_TIMEZONE
QString DatetimeModel::systemTimeZoneId() const
{
    return m_systemTimeZoneId;
}

void DatetimeModel::setSystemTimeZoneId(const QString &systemTimeZoneId)
{
    if (m_systemTimeZoneId != systemTimeZoneId) {
        m_systemTimeZoneId = systemTimeZoneId;
        Q_EMIT systemTimeZoneIdChanged(systemTimeZoneId);
    }
}
#endif

QList<ZoneInfo> DatetimeModel::userTimeZones() const
{
    return m_userTimeZones;
}

void DatetimeModel::addUserTimeZone(const ZoneInfo &zone)
{
    const QString zoneName = zone.getZoneName();

    if (!m_userZoneIds.contains(zoneName)) {
        m_userZoneIds.append(zoneName);
        m_userTimeZones.append(zone);
        Q_EMIT userTimeZoneAdded(zone);
    }
}

void DatetimeModel::removeUserTimeZone(const ZoneInfo &zone)
{
    const QString zoneName = zone.getZoneName();

    if (m_userZoneIds.contains(zoneName)) {
        m_userZoneIds.removeAll(zoneName);
        m_userTimeZones.removeAll(zone);
        Q_EMIT userTimeZoneRemoved(zone);
    }
}

void DatetimeModel::setCurrentTimeZone(const ZoneInfo &currentTimeZone)
{
    if (m_currentTimeZone == currentTimeZone)
        return;

    m_currentTimeZone = currentTimeZone;

    Q_EMIT currentTimeZoneChanged(currentTimeZone);
}

void DatetimeModel::setCurrentUseTimeZone(const ZoneInfo &currentSysTimeZone)
{
    if (m_currentSystemTimeZone == currentSysTimeZone)
        return;

    m_currentSystemTimeZone = currentSysTimeZone;

    Q_EMIT currentSystemTimeZoneChanged(currentSysTimeZone);
}

void DatetimeModel::setNtpServerAddress(const QString &ntpServer)
{
    if (m_strNtpServerAddress != ntpServer) {
        m_strNtpServerAddress = ntpServer;
        Q_EMIT NTPServerChanged(ntpServer);
    }
}

void DatetimeModel::setNTPServerList(const QStringList &list)
{
    if (m_NtpServerList != list) {
        m_NtpServerList = list;
        Q_EMIT NTPServerListChanged(list);
    }
}

void DatetimeModel::setTimeZoneInfo(const QString &timeZone)
{
    if (m_timeZones != timeZone) {
        m_timeZones = timeZone;
        Q_EMIT timeZoneChanged(timeZone);
    }
}

void DatetimeModel::setCountry(const QString &country)
{
    if (m_country != country) {
        m_country = country;
        Q_EMIT countryChanged(country);
    }
}

void DatetimeModel::setLocaleName(const QString &localeName)
{
    if (m_localeName != localeName) {
        m_localeName = localeName;
        Q_EMIT localeNameChanged(localeName);
    }
}

void DatetimeModel::setLangRegion(const QString &langCountry)
{
    if (m_langCountry != langCountry) {
        m_langCountry = langCountry;
        Q_EMIT langCountryChanged(langCountry);
    }
}

void DatetimeModel::setFirstDayOfWeek(const int &firstDayOfWeekFormat)
{
    if (m_firstDayOfWeekFormat != firstDayOfWeekFormat) {
        m_firstDayOfWeekFormat = firstDayOfWeekFormat;
        Q_EMIT firstDayOfWeekFormatChanged(firstDayOfWeekFormat);
    }
}

void DatetimeModel::setShortDateFormat(const QString &shortDateFormat)
{
    if (m_shortDateFormat != shortDateFormat) {
        m_shortDateFormat = shortDateFormat;
        Q_EMIT shortDateFormatChanged(shortDateFormat);
    }
}

void DatetimeModel::setLongDateFormat(const QString &longDateFormat)
{
    if (m_longDateFormat != longDateFormat) {
        m_longDateFormat = longDateFormat;
        Q_EMIT longDateFormatChanged(longDateFormat);
    }
}

void DatetimeModel::setShortTimeFormat(const QString &shortTimeFormat)
{
    if (m_shortTimeFormat != shortTimeFormat) {
        m_shortTimeFormat = shortTimeFormat;
        Q_EMIT shortTimeFormatChanged(shortTimeFormat);
    }
}

void DatetimeModel::setLongTimeFormat(const QString &longTimeFormat)
{
    if (m_longTimeFormat != longTimeFormat) {
        m_longTimeFormat = longTimeFormat;
        Q_EMIT longTimeFormatChanged(longTimeFormat);
    }
}

void DatetimeModel::setCurrencyFormat(const QString &currencyFormat)
{
    if (m_currencyFormat != currencyFormat) {
        m_currencyFormat = currencyFormat;
        Q_EMIT currencyFormatChanged(currencyFormat);
    }
}

void DatetimeModel::setNumberFormat(const QString &numberFormat)
{
    if (m_numberFormat != numberFormat) {
        m_numberFormat = numberFormat;
        Q_EMIT numberFormatChanged(numberFormat);
    }
}

void DatetimeModel::setPaperFormat(const QString &paperFormat)
{
    if (m_paperFormat != paperFormat) {
        m_paperFormat = paperFormat;
        Q_EMIT paperFormatChanged(paperFormat);
    }
}

void DatetimeModel::setRegionFormat(const RegionFormat &regionFormat)
{
    if (m_regionFormat != regionFormat) {
        m_regionFormat = regionFormat;
    }
}

void DatetimeModel::setCountries(const QStringList &countries)
{
    if (m_countries != countries) {
        m_countries = countries;
    }
}

void DatetimeModel::setRegions(const Regions &regions)
{
    if (m_regions != regions) {
        m_regions = regions;
    }
}

QStringList DatetimeModel::userTimeZoneText(int index) const
{
    QStringList userTimeZoneList;
    for (const ZoneInfo &zoneInfo : m_userTimeZones) {
        QString text;
        switch (index) {
        case 1:
            text = getDescription(zoneInfo);
            break;
        case 2:
            text = QString::number(zoneInfo.getUTCOffset());
            break;
        case 3:
            text = zoneInfo.getZoneName();
            break;
        case 4:
            text = zoneInfo.getZoneCity();
            break;
        default:
            text = getDisplayText(zoneInfo);
            break;
        }
        userTimeZoneList << text;
    }

    return userTimeZoneList;
}

QString DatetimeModel::currentTimeZoneName() const
{
    return getDescription(m_currentTimeZone);
}

QString DatetimeModel::timeZoneDispalyName() const
{
    return getDisplayText(m_currentSystemTimeZone);
}

int DatetimeModel::currentTimeZoneIndex() const
{
    using namespace installer;
    if (g_totalZones.empty())
        g_totalZones =  GetZoneInfoList();

    int index = -1;
    const QString &zoneName = m_currentSystemTimeZone.getZoneName();
    for (int i = 0; i < g_totalZones.size(); ++i) {
        const auto &zoneInfo = g_totalZones.value(i);
        if (zoneName == zoneInfo.timezone) {
            index = i;
            break;
        }
    }

    return index;
}

DCC_FACTORY_CLASS(DatetimeModel)

#include "datetimemodel.moc"
