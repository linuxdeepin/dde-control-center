//SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REGIONPROXY_H
#define REGIONPROXY_H

#include <QDate>
#include <QDebug>

const QString localeName_key = "localeName";
const QString country_key = "country";
const QString languageRegion_key = "languageRegion";
const QString firstDayOfWeek_key = "firstDayOfWeek";
const QString shortDateFormat_key = "shortDateFormat";
const QString longDateFormat_key = "longDateFormat";
const QString shortTimeFormat_key = "shortTimeFormat";
const QString longTimeFormat_key = "longTimeFormat";
const QString currencyFormat_key = "currencyFormat";
const QString numberFormat_key = "numberFormat";
const QString digitgroupFormat_key = "digitgroupFormat";
const QString paperFormat_key = "paperFormat";

struct RegionFormat {
    int firstDayOfWeekFormat = 0;
    QString shortDateFormat;
    QString longDateFormat;
    QString shortTimeFormat;
    QString longTimeFormat;
    QString currencyFormat;
    QString numberFormat;
    QString digitgroupFormat; //groupSeparator;
    QString paperFormat;
    bool operator!=(const RegionFormat &other)
    {
        return !(*this == other);
    }
    bool operator==(const RegionFormat &other)
    {
        return (this->firstDayOfWeekFormat == other.firstDayOfWeekFormat) &&
                (this->shortDateFormat == other.shortDateFormat) &&
                (this->longDateFormat == other.longDateFormat) &&
                (this->shortTimeFormat == other.shortTimeFormat) &&
                (this->longTimeFormat == other.longTimeFormat) &&
                (this->currencyFormat == other.currencyFormat) &&
                (this->numberFormat == other.numberFormat) &&
                (this->digitgroupFormat == other.digitgroupFormat) &&
                (this->paperFormat == other.paperFormat);
    }
};

QDebug operator<<(QDebug debug, const RegionFormat &regionFormat);

struct RegionAvailableData {
    QStringList daysAvailable;
    QStringList shortDatesAvailable;
    QStringList longDatesAvailable;
    QStringList shortTimesAvailable;
    QStringList longTimesAvailable;
    RegionAvailableData& operator+=(const RegionAvailableData &rhs) {
        for (QString element : rhs.daysAvailable) {
            if (daysAvailable.contains(element))
                continue;
            daysAvailable << element;
        }
        for (QString element : rhs.shortDatesAvailable) {
            if (shortDatesAvailable.contains(element))
                continue;
            shortDatesAvailable << element;
        }
        for (QString element : rhs.longDatesAvailable) {
            if (longDatesAvailable.contains(element))
                continue;
            longDatesAvailable << element;
        }
        for (QString element : rhs.shortTimesAvailable) {
            if (shortTimesAvailable.contains(element))
                continue;
            shortTimesAvailable << element;
        }
        for (QString element : rhs.longTimesAvailable) {
            if (longTimesAvailable.contains(element))
                continue;
            longTimesAvailable << element;
        }
        return *this;
    }
    void clear() {
        daysAvailable.clear();
        shortDatesAvailable.clear();
        longDatesAvailable.clear();
        shortTimesAvailable.clear();
        longTimesAvailable.clear();
    }
};

using Regions = QMap<QString, QLocale>;
class QTranslator;
class RegionProxy : public QObject
{
    Q_OBJECT
public:
    explicit RegionProxy(QObject *parent = nullptr);
    ~RegionProxy();

    Regions regions() const;
    QStringList countries() const;
    QString systemCountry() const;
    QString langCountry() const;

    static RegionFormat regionFormat(const QLocale &locale);
    static RegionFormat systemRegionFormat();
    static RegionAvailableData allTextData(const QLocale &locale);
    static RegionAvailableData allFormat();

    void active();

    bool isActive() { return m_isActive; }
private:
    QStringList m_countries;
    Regions m_regions;
    QTranslator *m_translatorLanguage = nullptr;
    QTranslator *m_translatorCountry = nullptr;
    static RegionAvailableData m_formatData;
    static RegionAvailableData m_allFormat, m_defaultFormat, m_customFormat;
    static RegionAvailableData customTextData(const QLocale &locale);
    static RegionAvailableData defaultTextData(const QLocale &locale);
    bool m_isActive;
};

#endif // REGIONPROXY_H
