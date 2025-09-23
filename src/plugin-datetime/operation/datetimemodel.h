//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>
#include <QPoint>
#include <QAbstractListModel>

#include "zoneinfo.h"
#include "regionproxy.h"
#include "zoneinfomodel.h"

class DatetimeWorker;
namespace dccV25 {
class KeyboardModel;
}
class DatetimeModel : public QObject
{
    Q_OBJECT
    friend class DatetimeWorker;
    Q_PROPERTY(bool ntpEnabled READ nTP WRITE setNTP NOTIFY ntpChanged FINAL)
    Q_PROPERTY(bool use24HourFormat READ use24HourFormat WRITE set24HourFormat NOTIFY hourTypeChanged FINAL)
    Q_PROPERTY(QString ntpServerAddress READ ntpServerAddress WRITE setNtpServerAddress NOTIFY NTPServerChanged FINAL)
    Q_PROPERTY(QString previousServerAddress READ previousServerAddress WRITE setPreviousServerAddress NOTIFY previousServerAddressChanged FINAL)
    Q_PROPERTY(QStringList ntpServerList READ ntpServerList WRITE setNTPServerList NOTIFY NTPServerListChanged FINAL)
    Q_PROPERTY(QString timeZoneDispalyName READ timeZoneDispalyName NOTIFY currentSystemTimeZoneChanged)
    Q_PROPERTY(int currentTimeZoneIndex READ currentTimeZoneIndex NOTIFY currentSystemTimeZoneChanged)
    Q_PROPERTY(QString systemTimeZone READ getTimeZone WRITE setTimeZoneInfo NOTIFY timeZoneChanged FINAL)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged FINAL)
    Q_PROPERTY(QStringList countries READ countries WRITE setCountries NOTIFY countriesChanged FINAL)
    Q_PROPERTY(QString region READ region WRITE setRegion NOTIFY regionChanged FINAL)
    Q_PROPERTY(int currentRegionIndex READ currentRegionIndex NOTIFY currentRegionIndexChanged FINAL)

    Q_PROPERTY(QStringList langList READ langList NOTIFY langListChanged FINAL)
    Q_PROPERTY(QString currentLang READ currentLang  NOTIFY currentLangChanged FINAL)

    Q_PROPERTY(QString langRegion READ langRegion WRITE setLangRegion NOTIFY langCountryChanged FINAL)

    Q_PROPERTY(int weekdayFormat READ weekdayFormat WRITE setWeekdayFormat NOTIFY weekdayFormatChanged FINAL)
    Q_PROPERTY(int firstDayOfWeek READ firstDayOfWeekFormat WRITE setFirstDayOfWeek NOTIFY firstDayOfWeekFormatChanged FINAL)
    Q_PROPERTY(QString shortDateFormat READ shortDateFormat WRITE setShortDateFormat NOTIFY shortDateFormatChanged FINAL)
    Q_PROPERTY(QString longDateFormat READ longDateFormat WRITE setLongDateFormat NOTIFY longDateFormatChanged FINAL)
    Q_PROPERTY(QString shortTimeFormat READ shortTimeFormat WRITE setShortTimeFormat NOTIFY shortTimeFormatChanged FINAL)
    Q_PROPERTY(QString longTimeFormat READ longTimeFormat WRITE setLongTimeFormat NOTIFY longTimeFormatChanged FINAL)

    Q_PROPERTY(QString currentLanguageAndRegion READ currentLanguageAndRegion NOTIFY currentLanguageAndRegionChanged FINAL)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY currentDateChanged FINAL)
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY currentTimeChanged FINAL)

    Q_PROPERTY(QString digitGroupingSymbol READ digitGroupingSymbol WRITE setDigitGroupingSymbol NOTIFY digitGroupingSymbolChanged FINAL)
    Q_PROPERTY(int langState READ langState NOTIFY langStateChanged FINAL)
    Q_PROPERTY(QString numberExampleFormat READ numberExampleFormat NOTIFY numberExampleFormatChanged FINAL)

public:
    using Regions = QMap<QString, QLocale>;
    enum Format {
        // date time formats
        DayAbbreviations,     // Monday/Mon ; 星期一 / 周一
        DayOfWeek,            // 一周首日
        ShortDate,            // 短日期
        LongDate,             // 长日期
        ShortTime,            // 短时间
        LongTime,             // 长时间

        // currency formats
        CurrencySymbol,       // 货币符号 ￥
        PositiveCurrency,     // 货币正数 ￥1.0
        NegativeCurrency,     // 货币负数 ￥-1.0

        // number formats
        DecimalSymbol,        // 小数点
        DigitGroupingSymbol,  // 分隔符（数字分组）
        DigitGrouping,        // 数字分组

        PageSize              // 纸张大小 A4
    };

    explicit DatetimeModel(QObject *parent = nullptr);
    ~DatetimeModel();

    inline bool nTP() const { return m_ntp; }
    void setNTP(bool ntp);
    inline bool use24HourFormat() const { return m_bUse24HourType; }

    QList<ZoneInfo> userTimeZones() const;
    void addUserTimeZone(const ZoneInfo &zone);
    void removeUserTimeZone(const ZoneInfo &zone);
#ifndef DCC_DISABLE_TIMEZONE
    QString systemTimeZoneId() const;
    void setSystemTimeZoneId(const QString &systemTimeZoneId);
#endif
    inline ZoneInfo currentTimeZone() const
    {
        return m_currentTimeZone;
    }
    void setCurrentTimeZone(const ZoneInfo &currentTimeZone);

    inline ZoneInfo currentSystemTimeZone() const { return m_currentSystemTimeZone; }
    void setCurrentUseTimeZone(const ZoneInfo &currentTimeZone);

    inline QString ntpServerAddress() const { return m_strNtpServerAddress; }
    void setNtpServerAddress(const QString &ntpServer);

    inline QString previousServerAddress() const { return m_previousServerAddress; }
    void setPreviousServerAddress(const QString &address);

    inline QStringList ntpServerList() const { return m_NtpServerList; }
    void setNTPServerList(const QStringList &list);

    inline QString getTimeZone() const { return m_timeZones; }
    void setTimeZoneInfo(const QString &timeZone);

    inline QString country() const { return m_country; }
    void setCountry(const QString &country);

    inline QString localeName() const { return m_localeName; }
    void setLocaleName(const QString &localeName);

    inline QString langRegion() const { return m_langCountry; }
    void setLangRegion(const QString &langCountry);

    inline int firstDayOfWeekFormat() const { return m_firstDayOfWeekFormat; }
    void setFirstDayOfWeek(const int &firstDayOfWeekFormat);

    inline QString shortDateFormat() const { return m_shortDateFormat; }
    void setShortDateFormat(const QString &shortDateFormat);

    inline QString longDateFormat() const { return m_longDateFormat; }
    void setLongDateFormat(const QString &longDateFormat);

    inline QString shortTimeFormat() const { return m_shortTimeFormat; }
    void setShortTimeFormat(const QString &shortTimeFormat);

    inline QString longTimeFormat() const { return m_longTimeFormat; }
    void setLongTimeFormat(const QString &longTimeFormat);

    inline QString currencyFormat() const { return m_currencyFormat; }
    void setCurrencyFormat(const QString &currencyFormat);

    QString digitGroupingSymbol() const { return m_digitGroupingSymbol; }
    void setDigitGroupingSymbol(const QString &digitGroupingSymbol);

    inline QString numberFormat() const { return m_numberFormat; }
    void setNumberFormat(const QString &numberFormat);

    inline QString paperFormat() const { return m_paperFormat; }
    void setPaperFormat(const QString &paperFormat);

    inline RegionFormat regionFormat() const { return m_regionFormat; }
    void setRegionFormat(const RegionFormat &regionFormat);

    inline QStringList countries() const { return m_countries; }
    void setCountries(const QStringList &countries);

    inline Regions regions() const { return m_regions; }
    void setRegions(const Regions &regions);

    QString timeZoneDispalyName() const;
    int currentTimeZoneIndex() const;

    QStringList langList();
    QString currentLang();

    int weekdayFormat() const;
    void setWeekdayFormat(int newWeekdayFormat);
    int langState() const;

    QString numberExampleFormat() const;

Q_SIGNALS:
    void ntpChanged(bool value);
    void hourTypeChanged(bool value);
    void userTimeZoneAdded(const ZoneInfo &zone);
    void userTimeZoneRemoved(const ZoneInfo &zone);
    void systemTimeZoneIdChanged(const QString &zone);
    void systemTimeChanged();
    void currentTimeZoneChanged(const ZoneInfo &zone) const;
    void currentSystemTimeZoneChanged(const ZoneInfo &zone) const;
    void NTPServerChanged(QString server);
    void NTPServerListChanged(QStringList list);
    void NTPServerNotChanged(QString server);
    void previousServerAddressChanged(const QString &address);
    void timeZoneChanged(QString value);
    void localeNameChanged(const QString &localeName);
    void countryChanged(const QString &country);
    void langCountryChanged(const QString &langCountry);
    void firstDayOfWeekFormatChanged(const int firstDayOfWeekFormat);
    void shortDateFormatChanged(const QString &shortDateFormat);
    void longDateFormatChanged(const QString &longDate);
    void shortTimeFormatChanged(const QString &shortTimeFormat);
    void longTimeFormatChanged(const QString &longTimeFormat);
    void currencyFormatChanged(const QString &oldFormat, const QString &newFormat);
    void digitGroupingSymbolChanged(const QString &oldFormat, const QString &newFormat);
    void numberFormatChanged(const QString &numberFormat);
    void paperFormatChanged(const QString &numberFormat);
    // Language List changed
    void langListChanged(const QStringList &langList);
    void currentLangChanged(const QString &lang);
    void currentLanguageAndRegionChanged(const QString &lang);
    void weekdayFormatChanged(int format);
    void symbolChanged(int format, const QString &symbol);
    void countriesChanged(const QStringList &countries);
    void regionChanged(const QString &region);
    void currentRegionIndexChanged(int index);
    void currentDateChanged();
    void currentTimeChanged();
    void currentFormatChanged(int format);
    void langStateChanged(int state);
    void numberExampleFormatChanged(const QString &numberExampleFormat);

public Q_SLOTS:
    void set24HourFormat(bool state);
    void setDateTime(const QDateTime &dateTime);
    QStringList zones(int x, int y, int map_width, int map_height);
    QPoint zonePosition(const QString &timezone, int map_width, int map_height);
    QStringList zoneIdList();
    QString zoneDisplayName(const QString &zoneName);
    QAbstractListModel *userTimezoneModel();
    QSortFilterProxyModel *zoneSearchModel();
    QSortFilterProxyModel *langSearchModel();
    QSortFilterProxyModel *langRegionSearchModel();
    QSortFilterProxyModel *regionSearchModel();
    QAbstractListModel *timeDateModel();
    QAbstractListModel *currencyModel();
    QAbstractListModel *decimalModel();
    QString region(); // country
    int currentRegionIndex();
    void setRegion(const QString &region); // setCountry
    void addUserTimeZoneById(const QString &zoneId);
    void removeUserTimeZoneById(const QString &name);
    void setSystemTimeZone(const QString &zoneId);
    QString timeZoneDescription(const ZoneInfo &zone) const;
    void ensureLangModel();
    void addLang(const QString &lang);
    void deleteLang(const QString &lang);
    void setCurrentLang(const QString &lang);

    QStringList languagesAndRegions();
    QString currentLanguageAndRegion();
    int currentLanguageAndRegionIndex();
    void setCurrentLocaleAndLangRegion(const QString &localeName, const QString& langAndRegion);

    // format ==> DatetimeModel::Format
    QStringList availableFormats(int format) const;
    int currentFormatIndex(int format) const;
    void setCurrentFormat(int format, int index);
    QString currentDate();
    QString currentTime() const;
    QString getCustomNtpServer() const;
protected:
    void initModes(const QStringList &names, int indexBegin, int indexEnd, QAbstractListModel *model);
private:
    bool m_ntp;
    bool m_bUse24HourType;
    QStringList m_userZoneIds;
#ifndef DCC_DISABLE_TIMEZONE
    QString m_systemTimeZoneId;
#endif
    QList<ZoneInfo> m_userTimeZones;
    ZoneInfo m_currentTimeZone;
    ZoneInfo m_currentSystemTimeZone;
    QString m_strNtpServerAddress;
    QString m_previousServerAddress;
    QStringList m_NtpServerList;
    QString m_timeZones;
    QString m_country;
    QString m_langCountry;
    QStringList m_countries;
    Regions m_regions;
    QString m_localeName;
    int m_firstDayOfWeekFormat;
    QString m_shortDateFormat;
    QString m_longDateFormat;
    QString m_shortTimeFormat;
    QString m_longTimeFormat;
    QString m_currencyFormat;
    QString m_digitGroupingSymbol;
    QString m_numberFormat;
    QString m_paperFormat;
    RegionFormat m_regionFormat;
    QMap<QString, QString> m_timezoneCache;
    DatetimeWorker *m_work = nullptr;
    dccV25::UserTimezoneModel *m_userTimezoneModel = nullptr;
    QSortFilterProxyModel *m_zoneSearchModel = nullptr;
    QSortFilterProxyModel *m_langSearchModel = nullptr;
    QSortFilterProxyModel *m_regionSearchModel = nullptr;
    QSortFilterProxyModel *m_countrySearchModel = nullptr;
    QAbstractListModel *m_timeDateModel = nullptr;
    QAbstractListModel *m_currencyModel = nullptr;
    QAbstractListModel *m_decimalModel = nullptr;
    dccV25::KeyboardModel *m_langModel = nullptr;
    QMap<QString, QString> m_langRegionsCache;
    QString m_regionName;
};

#endif // DATETIMEMODEL_H
