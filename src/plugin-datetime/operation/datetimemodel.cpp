//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimemodel.h"
#include "dccfactory.h"
#include "timezoneMap/timezone_map_util.h"
#include "datetimeworker.h"
#include "zoneinfomodel.h"
#include "keyboard/keyboardmodel.h"
#include "languagelistmodel.h"
#include "langregionmodel.h"

#include <unicode/locid.h>
#include <unicode/unistr.h>

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>
#include <QCoreApplication>
#include <QStringListModel>
#include <QTimer>

static installer::ZoneInfoList g_totalZones;

static QString GetChinaTaiWanName()
{
    switch (QLocale::system().territory()) {
    case QLocale::Territory::China:
        return QStringLiteral("中国台湾");
    case QLocale::Territory::HongKong:
        return QStringLiteral("中國台灣");
    case QLocale::Territory::Taiwan:
        return QStringLiteral("中國臺灣");
    case QLocale::Territory::Macau:
        return QStringLiteral("中國台灣");
    default:
        return "Taiwan China";
    }
}

static const QMap<QString, QMap<QLocale::Language, QString>> localeOverrides = {
    {"zh_TW", {
        {QLocale::Chinese, GetChinaTaiWanName()},
        {QLocale::English, "Taiwan China"},
    }},
    {"trv_TW", {
        {QLocale::Chinese, GetChinaTaiWanName()},
        {QLocale::English, "Taiwan China"}
    }},
};

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

    return QString("%1, %2").arg(dateLiteral).arg(description);
}

static QString getDisplayText(const ZoneInfo &zoneInfo)
{
    QString gmData = zoneInfo.getUtcOffsetText();
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

static inline QStringList getCurrencySymbol(bool positive, const QString &symbol)
{
    const QString money("1.1");
    if (positive)
        return {
            QString("%1%2").arg(symbol).arg(money),  // ￥1.1
            QString("%1%2").arg(money).arg(symbol),  // ￥1.1
            QString("%1 %2").arg(symbol).arg(money), // ￥ 1.1
            QString("%1 %2").arg(money).arg(symbol)  // 1.1 ￥
        };

    return {
        QString("-%1%2").arg(symbol).arg(money), // -￥1.1
        QString("%1-%2").arg(symbol).arg(money), // ￥-1.1
        QString("%1%2-").arg(symbol).arg(money), // ￥1.1-
        QString("-%1%2").arg(money).arg(symbol), // 1.1-￥
        QString("%1-%2").arg(money).arg(symbol), // 1.1-￥
        QString("%1%2-").arg(money).arg(symbol)  // 1.1￥-
    };
}

static inline QString escapSpace(const QString &space)
{
    if (space.isEmpty())
        return QLatin1String(" ");
    // 不同语言下空格可能不同。。详情看 QChar::isSpace 实现
    bool isSpace = space.at(0).isSpace() || space == DatetimeModel::tr("Space");
    return isSpace ? QLatin1String(" ") : space;
}

static inline QString normalizeSpace(const QString &value)
{
    QString ret = " ";
    QStringList numberKeepList{ QString("."), QString(","), QString("'")};
    if (numberKeepList.contains(value)) {
        ret = value;
    } else {
        if (value == "Space" || value == DatetimeModel::tr("Space")) {
            return ret;
        }
    }
    return ret;
}

static inline QString unEscapSpace(const QString &space)
{
    if (space.isEmpty())
        return QLatin1String(" ");

    return space.at(0).isSpace() ? DatetimeModel::tr("Space") : space;
}

static inline QStringList separatorSymbol(const QLocale &locale, bool grouping)
{
    QStringList symbols{ QString("."), QString(","), QString("'"), DatetimeModel::tr("Space") };
    QString separator = grouping ? locale.groupSeparator() : locale.decimalPoint();
    separator = unEscapSpace(separator);

    if (!symbols.contains(separator))
        symbols.prepend(separator);

    return symbols;
}

static QStringList translateLangAndCountry(const QString &localeName)
{
    auto localeSystem = QLocale::system();
    auto systemLocale = icu::Locale(localeSystem.name().toStdString().data());
    auto IcuLocale = icu::Locale(localeName.toStdString().data());
    auto localeHex = icu::UnicodeString(localeName.toStdString().data());
    std::string displayLanguageIcu;
    IcuLocale.getDisplayLanguage(systemLocale, localeHex).toUTF8String(displayLanguageIcu);
    std::string displayCountryIcu;
    IcuLocale.getDisplayCountry(systemLocale, localeHex).toUTF8String(displayCountryIcu);

    // override icu， e.g. 台湾 -> 中国台湾
    if (localeOverrides.contains(localeName)) {
        if (localeOverrides[localeName].contains(QLocale::system().language())) {
            displayCountryIcu = localeOverrides[localeName][QLocale::system().language()].toStdString();
        }
    }

    return QStringList{ QString::fromStdString(displayLanguageIcu),
                        QString::fromStdString(displayCountryIcu) };
}

static QString translate(const QString &localeName, const QString &langRegion)
{
    QStringList langRegions = langRegion.split(":");
    if (langRegions.size() < 2) {
        return langRegion;
    }

    if (langRegions[0] == "Traditional Chinese" || langRegions[0] == "Simplified Chinese"
        || langRegions[1] == QLocale::countryToString(QLocale::HongKong)
        || langRegions[1] == QLocale::countryToString(QLocale::Macau)
        || langRegions[1] == QLocale::countryToString(QLocale::Taiwan)) {

        auto res = translateLangAndCountry(localeName);
        QString lang = res.value(0);

        // override icu， e.g. 台湾 -> 中国台湾
        QString country = langRegions.at(1).toUtf8().data();
        if (country == "Taiwan") {
            country = GetChinaTaiWanName();
        } else {
            country = QCoreApplication::translate("dcc::datetime::Country", langRegions.at(1).toUtf8().data());
        }

        QString langCountry = QString("%1(%2)").arg(lang).arg(country);
        return langCountry;
    }

    auto res = translateLangAndCountry(localeName);
    QString langCountry = QString("%1(%2)").arg(res.value(0)).arg(res.value(1));

    return langCountry;
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

    // set timezone
    // connect(this, &DatetimeModel::timeZoneChanged, m_work, &DatetimeWorker::setTimezone);

    connect(this, &DatetimeModel::currencyFormatChanged, this, [this](const QString &oldFormat, const QString &newFormat){
        // get PositiveCurrency/NegativeCurrency
        auto posFmt = m_work->positiveCurrencyFormat();
        auto negFmt = m_work->negativeCurrencyFormat();

        m_work->setPositiveCurrencyFormat(posFmt.replace(oldFormat, newFormat));
        m_work->setNegativeCurrencyFormat(negFmt.replace(oldFormat, newFormat));
    });
    connect(this, &DatetimeModel::digitGroupingSymbolChanged, this, [this](const QString &oldFormat, const QString &newFormat){
        QString fmt1 = escapSpace(oldFormat);
        QString fmt2 = escapSpace(newFormat);
        auto digitGrouping = m_work->digitGrouping();
        m_work->setDigitGrouping(digitGrouping.replace(fmt1, fmt2));
    });

    connect(this, &DatetimeModel::symbolChanged, this, [this](int format, const QString &symbol) {
        if (format != CurrencySymbol && format != DigitGroupingSymbol)
            return;

        Q_EMIT currentFormatChanged(format);
    });

    connect(this, &DatetimeModel::currentLanguageAndRegionChanged, this, [this]() {
        Q_EMIT currentFormatChanged(-1);
    });
    connect(this, &DatetimeModel::shortDateFormatChanged, this, [this]() {
        Q_EMIT currentFormatChanged(ShortDate);
    });
    connect(this, &DatetimeModel::longDateFormatChanged, this, [this]() {
        Q_EMIT currentFormatChanged(LongDate);
    });
    connect(this, &DatetimeModel::shortTimeFormatChanged, this, [this]() {
        Q_EMIT currentFormatChanged(ShortTime);
    });
    connect(this, &DatetimeModel::longTimeFormatChanged, this, [this]() {
        Q_EMIT currentFormatChanged(LongTime);
    });

    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, [this](){
        Q_EMIT currentTimeChanged();

        // maybe too frequently
        Q_EMIT currentDateChanged();
    });
    timer->start();
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
        QString utcOffsetText = zoneInfo.getUtcOffsetText();
        QString cityName = zoneInfo.getZoneCity().isEmpty() ? zoneInfo.getZoneName() : zoneInfo.getZoneCity();
        return QString("%1 %2").arg(utcOffsetText).arg(cityName);
    }
    return QString();
}

QAbstractListModel *DatetimeModel::userTimezoneModel()
{
    if (m_userTimezoneModel)
        return m_userTimezoneModel;

    m_userTimezoneModel = new dccV25::UserTimezoneModel(this);
    connect(this, &DatetimeModel::userTimeZoneAdded, m_userTimezoneModel, &dccV25::UserTimezoneModel::reset);
    connect(this, &DatetimeModel::userTimeZoneRemoved, m_userTimezoneModel, &dccV25::UserTimezoneModel::reset);
    connect(this, &DatetimeModel::timeZoneChanged, m_userTimezoneModel, [this]() {
        auto indexBegin = m_userTimezoneModel->index(0);
        auto indexEnd = m_userTimezoneModel->index(m_userTimeZones.count() - 1);
        Q_EMIT m_userTimezoneModel->dataChanged(indexBegin, indexEnd);
    });

    return m_userTimezoneModel;
}

QSortFilterProxyModel *DatetimeModel::zoneSearchModel()
{
    if (m_zoneSearchModel)
        return m_zoneSearchModel;

    m_zoneSearchModel = new QSortFilterProxyModel(this);

    auto sourceModel = new dccV25::ZoneInfoModel(this);
    m_zoneSearchModel->setSourceModel(sourceModel);
    m_zoneSearchModel->setFilterRole(dccV25::ZoneInfoModel::SearchTextRole);
    m_zoneSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_zoneSearchModel;
}

QSortFilterProxyModel *DatetimeModel::langSearchModel()
{
    if (m_langSearchModel)
        return m_langSearchModel;

    m_langSearchModel = new QSortFilterProxyModel(this);

    ensureLangModel();

    auto sourceModel = new dccV25::LanguageListModel(this);
    sourceModel->setMetaData(m_langModel->langLists());
    sourceModel->setLocalLang(m_langModel->localLang());
    connect(m_langModel, &dccV25::KeyboardModel::langChanged, sourceModel, &dccV25::LanguageListModel::setMetaData);
    connect(m_langModel, &dccV25::KeyboardModel::curLocalLangChanged, sourceModel, &dccV25::LanguageListModel::setLocalLang);

    m_langSearchModel->setSourceModel(sourceModel);
    m_langSearchModel->setFilterRole(dccV25::LanguageListModel::SearchTextRole);
    m_langSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_langSearchModel;
}

QSortFilterProxyModel *DatetimeModel::langRegionSearchModel()
{
    if (m_regionSearchModel)
        return m_regionSearchModel;

    m_regionSearchModel = new QSortFilterProxyModel(this);

    auto sourceModel = new dccV25::LangRegionModel(this);
    m_regionSearchModel->setSourceModel(sourceModel);
    m_regionSearchModel->setFilterRole(dccV25::ZoneInfoModel::SearchTextRole);
    m_regionSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_regionSearchModel;
}

QSortFilterProxyModel *DatetimeModel::regionSearchModel()
{
    if (m_countrySearchModel)
        return m_countrySearchModel;

    for (const auto &locale : m_regions) {
        auto langCountry = translateLangAndCountry(locale.name());
        // { 中国: CN }
        m_langRegionsCache[langCountry.value(1)] = locale.territoryToCode(locale.territory());
    }

    m_countrySearchModel = new QSortFilterProxyModel(this);
    QStringListModel *sourceModel = new QStringListModel(m_langRegionsCache.keys(), m_countrySearchModel);
    m_countrySearchModel->setSourceModel(sourceModel);
    m_countrySearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_countrySearchModel;
}

void DatetimeModel::initModes(const QStringList &names, int indexBegin, int indexEnd, QAbstractListModel *model)
{
    auto m = dynamic_cast<dccV25::FormatsModel *>(model);
    if (!m)
        return;

    QList<dccV25::FormatsInfo> datas;
    for (int i = indexBegin; i <= indexEnd && (i - indexBegin) < names.count(); ++i) {
        dccV25::FormatsInfo info;
        info.name = names[i - indexBegin];
        info.values = availableFormats(i);
        info.index = currentFormatIndex(i);
        info.indexBegin = indexBegin;
        datas << info;
    }

    m->setDatas(datas);
}

QAbstractListModel *DatetimeModel::timeDateModel()
{
    if (m_timeDateModel)
        return m_timeDateModel;

    auto model = new dccV25::FormatsModel(this);
    QStringList names = { tr("Week"), tr("First day of week"), tr("Short date"),
                          tr("Long date"), tr("Short time"), tr("Long time") };

    auto initFormattedModes = [this, model](const QStringList &names) {
        QStringList nameList = names;
        int indexBegin = DayAbbreviations;
        QStringList langRegions = langRegion().split(":");
        if (langRegions.size() >= 2 && !langRegions.first().contains("Chinese", Qt::CaseInsensitive)) {
            indexBegin += 1;
            nameList.removeFirst();
        }
        initModes(nameList, indexBegin, LongTime, model);
    };

    initFormattedModes(names);
    connect(this, &DatetimeModel::currentFormatChanged, model, [model, names, this, initFormattedModes](int format){
        if (format >= DayAbbreviations && format <= LongTime || format < 0) {
            initFormattedModes(names);
        }
    });

    m_timeDateModel = model;
    return m_timeDateModel;
}

QAbstractListModel *DatetimeModel::currencyModel()
{
    if (m_currencyModel)
        return m_currencyModel;

    auto model = new dccV25::FormatsModel(this);
    QStringList names = { tr("Currency symbol"), tr("Positive currency"), tr("Negative currency") };

    initModes(names, CurrencySymbol, NegativeCurrency, model);
    connect(this, &DatetimeModel::currentFormatChanged, model, [model, names, this](int format){
        if (format >= CurrencySymbol && format <= NegativeCurrency || format < 0)
            initModes(names, CurrencySymbol, NegativeCurrency, model);
    });
    m_currencyModel = model;
    return m_currencyModel;
}

QAbstractListModel *DatetimeModel::decimalModel()
{
    if (m_decimalModel)
        return m_decimalModel;

    auto model = new dccV25::FormatsModel(this);
    QStringList names = { tr("Decimal symbol"), tr("Digit grouping symbol"),
                          tr("Digit grouping"), tr("Page size") };

    initModes(names, DecimalSymbol, PageSize, model);
    connect(this, &DatetimeModel::currentFormatChanged, model, [model, names, this](int format){
        if (format >= DecimalSymbol && format <= PageSize || format < 0)
            initModes(names, DecimalSymbol, PageSize, model);
    });
    m_decimalModel = model;
    return m_decimalModel;
}

QString DatetimeModel::region()
{
    if (m_regionName.isEmpty()) {
        QString localeName;
        for (const auto &locale : m_regions) {
            if (locale.territoryToString(locale.territory()) == m_country) {
                localeName = locale.name();
                break;
            }
            if (locale.territoryToCode(locale.territory()) == m_country) {
                localeName = locale.name();
                break;
            }
        }
        auto langCountry = translateLangAndCountry(localeName);
        m_regionName = langCountry.value(1);
    }

    return m_regionName;
}

int DatetimeModel::currentRegionIndex()
{
    return m_langRegionsCache.keys().indexOf(region());
}

void DatetimeModel::setRegion(const QString &region)
{
    if (m_regionName == region)
        return;

    m_regionName = region;
    auto reg = m_langRegionsCache.value(region, region);
    QLocale locale(QLocale::C);
    for (const auto &tmplocale : m_regions) {
        if (tmplocale.territoryToCode(tmplocale.territory()) == reg) {
            if (m_work->genLocale(tmplocale.name())) {
                locale = tmplocale;
                qDebug() << "set locale success:" << locale.name();
                break;
            }
            if (locale.language() == QLocale::C) {
                locale = tmplocale;
            }
        }
    }
    if (locale.language() != QLocale::C) {
        qDebug() << "set locale:" << locale.name();
        QString country = locale.countryToString(locale.country());
        QString language = QLocale::languageToString(locale.language());
        QString langCountry = QString("%1:%2").arg(language).arg(country);
        m_work->setConfigValue(country_key, country);

        Q_EMIT regionChanged(region);
        Q_EMIT currentRegionIndexChanged(currentRegionIndex());
    }
}

QStringList DatetimeModel::languagesAndRegions()
{
    QStringList langAndRegions;
    for (auto locale : m_regions) {
        const QString &langCountry = translate(locale.name(), m_regions.key(locale));
        langAndRegions << langCountry;
    }

    return langAndRegions;
}

QString DatetimeModel::currentLanguageAndRegion()
{
    return translate(localeName(), langRegion());
}

void DatetimeModel::setCurrentLocaleAndLangRegion(const QString &localeName, const QString& langAndRegion)
{
    QStringList langRegions = langAndRegion.split(":");
    if (langRegions.size() < 2) {
        qWarning() << "invalid langAndRegion" << langAndRegion;
        return;
    }

    if (!m_work)
        return;

    QLocale locale(localeName);
    m_work->setConfigValue(languageRegion_key, langAndRegion);
    m_work->setConfigValue(localeName_key, localeName);

    RegionFormat regionFormat = RegionProxy::regionFormat(locale);
    // case FirstDayOfWeek:
    m_work->setConfigValue(firstDayOfWeek_key, regionFormat.firstDayOfWeekFormat);
    // case ShortDate:
    m_work->setConfigValue(shortDateFormat_key, regionFormat.shortDateFormat);
    // case LongDate:
    m_work->setConfigValue(longDateFormat_key, regionFormat.longDateFormat);
    // case ShortTime:
    m_work->setConfigValue(shortTimeFormat_key, regionFormat.shortTimeFormat);
    // case LongTime:
    m_work->setConfigValue(longTimeFormat_key, regionFormat.longTimeFormat);
    // case Currency:
    m_work->setConfigValue(currencyFormat_key, regionFormat.currencyFormat.toUtf8());
    m_work->setCurrencySymbol(locale.currencySymbol());
    // case Digit:
    m_work->setConfigValue(numberFormat_key, regionFormat.numberFormat.toUtf8());
    m_work->setDigitGrouping(regionFormat.numberFormat.toUtf8());
    m_work->setDigitGroupingSymbol(unEscapSpace(regionFormat.digitgroupFormat));
    // case PaperSize:
    m_work->setConfigValue(paperFormat_key, regionFormat.paperFormat.toUtf8());
    m_work->genLocale(locale.name());
}

QStringList DatetimeModel::availableFormats(int format)
{
    QLocale locale(m_localeName);
    RegionAvailableData regionFormatsAvailable = RegionProxy::allTextData(locale);
    switch (format) {
    // date time formats
    case DayAbbreviations:
        return QStringList{ locale.standaloneDayName(1, QLocale::LongFormat), locale.standaloneDayName(1, QLocale::ShortFormat) };
    case DayOfWeek: {
        QStringList days;
        for (int i = 1; i < 8; ++i)
            days << locale.standaloneDayName(i, QLocale::LongFormat);
        return days;
    }
    case LongDate:
        return regionFormatsAvailable.longDatesAvailable;
    case ShortDate:
        return regionFormatsAvailable.shortDatesAvailable;
    case LongTime:
        return regionFormatsAvailable.longTimesAvailable;
    case ShortTime:
        return regionFormatsAvailable.shortTimesAvailable;
    // currency formats
    case CurrencySymbol: {
        QStringList defaultSymbols { QString::fromLocal8Bit("¥"),
                                QString::fromLocal8Bit("$"),
                                QString::fromLocal8Bit("€") };
        const QString &current = RegionProxy::regionFormat(locale).currencyFormat;
        if (!defaultSymbols.contains(current))
            defaultSymbols.prepend(current);

        return defaultSymbols;
    }
    case PositiveCurrency: {
        return getCurrencySymbol(true, currencyFormat());
    }
    case NegativeCurrency: {
        return getCurrencySymbol(false, currencyFormat());
    }
    // number formats
    case DecimalSymbol:{
        return separatorSymbol(locale, false);
    }
    case DigitGroupingSymbol: {
        return separatorSymbol(locale, true);
    }
    case DigitGrouping: {
        QString dgSymbol = escapSpace(normalizeSpace(m_work->digitGroupingSymbol()));
        // 不带数字分隔符，方便自定义追加
        locale.setNumberOptions(QLocale::OmitGroupSeparator);
        // 有的国家使用的是阿拉伯*文*数字（东阿拉伯数字）
        // 如伊朗、阿富汗、巴基斯坦及印度部分地区  ١٢٣٤٥٦٧٨٩
        // https://zh.wikipedia.org/wiki/%E9%98%BF%E6%8B%89%E4%BC%AF%E6%96%87%E6%95%B0%E5%AD%97
        const QString numString = locale.toString(123456789);
        return {
            numString,                                                                      // 123456789
            QString(numString).insert(3, dgSymbol).insert(7, dgSymbol),                     // 123,456,789
            QString(numString).insert(6, dgSymbol),                                         // 123456,789
            QString(numString).insert(2, dgSymbol).insert(5, dgSymbol).insert(8, dgSymbol), // 12,34,56,789
        };
    }
    case PageSize:
        return {"A4"};
    default:
        break;
    }

    return QStringList();
}

int DatetimeModel::currentFormatIndex(int format)
{
#define INDEX_OF(format, MEMBER, isDate) { \
        const QDate CurrentDate(2024, 1, 1); \
        const QTime CurrentTime(1, 1, 1); \
        QLocale locale(m_localeName); \
        RegionAvailableData regionFormatsAvailable = RegionProxy::allTextData(locale); \
        const auto &fmt = isDate ? locale.toString(CurrentDate, format) : locale.toString(CurrentTime, format); \
        return regionFormatsAvailable.MEMBER.indexOf(fmt); \
    }

    switch (format) {
    // date time formats
    case DayAbbreviations:
        return weekdayFormat();
    case DayOfWeek: {
        return firstDayOfWeekFormat() - 1; // combo index start from 0
    }
    case LongDate: {
        INDEX_OF(longDateFormat(), longDatesAvailable, true);
    }
    case ShortDate: {
        INDEX_OF(shortDateFormat(), shortDatesAvailable, true);
    }
    case LongTime:{
        INDEX_OF(longTimeFormat(), longTimesAvailable, false);
    }
    case ShortTime: {
        INDEX_OF(shortTimeFormat(), shortTimesAvailable, false);
    }
    // currency formats
    case CurrencySymbol: {
        const QString &currencySymbol = currencyFormat();
        QStringList defaultSymbols = availableFormats(format);
        return defaultSymbols.indexOf(currencySymbol);
    }
    case PositiveCurrency: {
        auto fmt = m_work->positiveCurrencyFormat();
        auto fmts = getCurrencySymbol(true, currencyFormat());
        return fmts.indexOf(fmt);
    }
    case NegativeCurrency: {
        auto fmt = m_work->negativeCurrencyFormat();
        auto fmts = getCurrencySymbol(false, currencyFormat());
        return fmts.indexOf(fmt);
    }
    // number formats
    case DecimalSymbol: {
        const QString &current = m_work->decimalSymbol();
        QStringList defaultSymbols = separatorSymbol(QLocale(m_localeName), false);
        int index = defaultSymbols.indexOf(current);
        return (index != -1) ? index : defaultSymbols.indexOf(DatetimeModel::tr("Space"));
    }
    case DigitGroupingSymbol: {
        const QString &current = m_work->digitGroupingSymbol();
        QStringList defaultSymbols = separatorSymbol(QLocale(m_localeName), true);
        int index = defaultSymbols.indexOf(unEscapSpace(current));
        return (index != -1) ? index : defaultSymbols.indexOf(DatetimeModel::tr("Space"));
    }
    case DigitGrouping: {
        const QString &current = m_work->digitGrouping();
        QStringList defaultSymbols = availableFormats(format);
        auto index = defaultSymbols.indexOf(current);
        return index;
    }
    default:
        break;
    }

    return 0;
}

void DatetimeModel::setCurrentFormat(int format, int index)
{
    if (index < 0) {
        qWarning() << "Invalide index!";
        return;
    }

    RegionAvailableData regionFormat = RegionProxy::allFormat();
    QLocale locale(m_localeName);
    // const QString &symbol = RegionProxy::regionFormat(locale).currencyFormat;

    auto setConfig = [this](int index, const QString &key, const QStringList &availableList) {
        if (index < availableList.count()) {
            m_work->setConfigValue(key, availableList.at(index));
        } else {
            qWarning() << "Set [" << key << "] faild, invalid index" << index << availableList.count();
        }
    };

    switch (format) {
    // date time formats
    case DayAbbreviations: {
        setWeekdayFormat(index);
        break;
    }
    case DayOfWeek: {
        // dconfig
        m_work->setConfigValue(firstDayOfWeek_key, index + 1);
        // dbus
        m_work->setWeekStartDayFormat(index + 1);
        setFirstDayOfWeek(index + 1);
        break;
    }
    case LongDate: {
        setConfig(index, longDateFormat_key, regionFormat.longDatesAvailable);
        break;
    }
    case ShortDate: {
        setConfig(index, shortDateFormat_key, regionFormat.shortDatesAvailable);
        break;
    }
    case LongTime:{
        setConfig(index, longTimeFormat_key, regionFormat.longTimesAvailable);
        break;
    }
    case ShortTime: {
        setConfig(index, shortTimeFormat_key, regionFormat.shortTimesAvailable);
        break;
    }
    // currency formats
    case CurrencySymbol: {
        QStringList defaultSymbols = availableFormats(format);
        if (index >= defaultSymbols.count())
            return;

        // dconfig
        setConfig(index, currencyFormat_key, defaultSymbols);
        // dbus
        m_work->setCurrencySymbol(defaultSymbols.value(index));
    }
    break;
    case PositiveCurrency: {
        auto fmts = getCurrencySymbol(true, currencyFormat());
        if (index < fmts.count())
            m_work->setPositiveCurrencyFormat(fmts.value(index));
    }
    break;
    case NegativeCurrency: {
        auto fmts = getCurrencySymbol(false, currencyFormat());
        if (index < fmts.count())
            m_work->setNegativeCurrencyFormat(fmts.value(index));
    }
    break;
    // number formats
    case DecimalSymbol: {
        auto fmts = separatorSymbol(locale, false);
        if (index < fmts.count())
            m_work->setDecimalSymbol(fmts.value(index));
    }
    break;
    case DigitGroupingSymbol: {
        auto fmts = separatorSymbol(locale, true);
        if (index < fmts.count())
            m_work->setDigitGroupingSymbol(fmts.value(index));
    }
    break;
    case DigitGrouping: {
        QStringList fmts = availableFormats(format);
        if (index >= fmts.count())
            return

        // dconfig
        setConfig(index, numberFormat_key, fmts);
        // dbus
        m_work->setDigitGrouping(fmts.value(index));
    }
    break;
    default:
        break;
    }

    Q_EMIT currentFormatChanged(format);
}

QString DatetimeModel::currentDate()
{
    QLocale locale(QLocale::system().name());
    QString week = weekdayFormat() == 1 ? "ddd" : "dddd";
    QString dateFormat = shortDateFormat() + " " + week;

    return locale.toString(QDate::currentDate(), dateFormat);
}

QString DatetimeModel::currentTime() const
{
    QLocale locale(QLocale::system().name());
    QString timeFormat = longTimeFormat();
    // remove all occurrences of 't' and '[tttt]' or similar patterns
    timeFormat.remove(QRegularExpression("(\\[t+?\\]|t+)"));
    return locale.toString(QTime::currentTime(), timeFormat);
}

int DatetimeModel::currentLanguageAndRegionIndex()
{
    return m_regions.keys().indexOf(m_langCountry);
}

void DatetimeModel::addUserTimeZoneById(const QString &zoneId)
{
    if (zoneId.isEmpty() || !m_work)
        return;

    m_work->addUserTimeZone(zoneId);
}

void DatetimeModel::removeUserTimeZoneById(const QString &zoneId)
{
    if (zoneId.isEmpty() || !m_work)
        return;

    m_work->removeUserTimeZone(zoneId);
}

void DatetimeModel::setSystemTimeZone(const QString &zoneId)
{
    if (zoneId.isEmpty() || !m_work)
        return;

    m_work->setTimezone(zoneId);
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

    if (!m_userZoneIds.contains(zoneName) && zoneName != m_currentSystemTimeZone.getZoneName()) {
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
        Q_EMIT currentLanguageAndRegionChanged(currentLanguageAndRegion());
    }
}

void DatetimeModel::setLangRegion(const QString &langCountry)
{
    if (m_langCountry != langCountry) {
        m_langCountry = langCountry;
        Q_EMIT langCountryChanged(langCountry);
        Q_EMIT currentLanguageAndRegionChanged(currentLanguageAndRegion());
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
        QString oldFormat = m_currencyFormat;
        m_currencyFormat = currencyFormat;
        Q_EMIT currencyFormatChanged(oldFormat, currencyFormat);
    }
}

void DatetimeModel::setDigitGroupingSymbol(const QString &digitGroupingSymbol)
{
    if (m_digitGroupingSymbol == digitGroupingSymbol)
        return;

    QString oldFormat = m_digitGroupingSymbol;
    m_digitGroupingSymbol = digitGroupingSymbol;

    Q_EMIT digitGroupingSymbolChanged(oldFormat, digitGroupingSymbol);
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
        Q_EMIT countriesChanged(countries);
    }
}

void DatetimeModel::setRegions(const Regions &regions)
{
    if (m_regions != regions) {
        m_regions = regions;
    }
}

QString DatetimeModel::timeZoneDescription(const ZoneInfo &zone) const
{
    return getDescription(zone);
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

void DatetimeModel::ensureLangModel()
{
    if (m_langModel)
        return;

    m_langModel = new dccV25::KeyboardModel(this);
    connect(m_langModel, &dccV25::KeyboardModel::curLocalLangChanged, this, &DatetimeModel::langListChanged);
    connect(m_langModel, &dccV25::KeyboardModel::curLangChanged, this, &DatetimeModel::currentLangChanged);
    connect(m_langModel, &dccV25::KeyboardModel::onSetCurLangFinish, this, &DatetimeModel::langStateChanged);
}

void DatetimeModel::addLang(const QString &lang)
{
    ensureLangModel();
    m_langModel->addLang(lang);
}

void DatetimeModel::deleteLang(const QString &lang)
{
    ensureLangModel();
    m_langModel->deleteLang(lang);
}

void DatetimeModel::setCurrentLang(const QString &lang)
{
    ensureLangModel();
    m_langModel->doSetLang(lang);
}

QStringList DatetimeModel::langList()
{
    ensureLangModel();

    if (m_langModel)
        return m_langModel->localLang();

    return {};
}

QString DatetimeModel::currentLang()
{
    ensureLangModel();

    if (m_langModel)
        return m_langModel->curLang();

    return {};
}

int DatetimeModel::weekdayFormat() const
{
    return m_work ? m_work->weekdayFormat() : 0;
}

void DatetimeModel::setWeekdayFormat(int newWeekdayFormat)
{
    if (!m_work)
        return;

    m_work->setWeekdayFormat(newWeekdayFormat);
}

int DatetimeModel::langState() const
{
    if (!m_langModel)
        return 0;

    return m_langModel->getLangChangedState();
}

DCC_FACTORY_CLASS(DatetimeModel)

#include "datetimemodel.moc"
