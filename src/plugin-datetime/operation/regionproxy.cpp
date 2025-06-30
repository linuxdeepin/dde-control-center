// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "regionproxy.h"

#include <QCoreApplication>
#include <QTranslator>

static const QDate CurrentDate(2024, 1, 1);
static const QTime CurrentTime(1, 1, 1);

class Format
{
    enum Type { Date, Time };

public:
    virtual ~Format() = default;

    inline QStringList daysText()
    {
        return QStringList() << m_locale.dayName(Qt::Monday) << m_locale.dayName(Qt::Tuesday)
                             << m_locale.dayName(Qt::Wednesday) << m_locale.dayName(Qt::Thursday)
                             << m_locale.dayName(Qt::Friday) << m_locale.dayName(Qt::Saturday)
                             << m_locale.dayName(Qt::Sunday);
    }

    inline QStringList shortDatesText() { return textFromFormat(Date, shortDateFormats()); }

    inline QStringList longDatesText() { return textFromFormat(Date, longDateFormats()); }

    inline QStringList shortTimesText() { return textFromFormat(Time, shortTimeFormats()); }

    inline QStringList longTimesText() { return textFromFormat(Time, longTimeFormats()); }

    virtual QStringList shortDateFormats() { return QStringList(); }

    virtual QStringList longDateFormats() { return QStringList(); }

    virtual QStringList shortTimeFormats() { return QStringList(); }

    virtual QStringList longTimeFormats() { return QStringList(); }

    void updateState(QDate date, QTime time, QLocale locale)
    {
        m_date = date;
        m_time = time;
        m_locale = locale;
    }

private:
    QStringList textFromFormat(Type type, const QStringList &formats)
    {
        QStringList text;
        for (const QString &format : formats)
            type == Date ? text << m_locale.toString(m_date, format)
                         : text << m_locale.toString(m_time, format);
        return text;
    }

private:
    QDate m_date;
    QTime m_time;

protected:
    QLocale m_locale;
};

class DefaultFormat : public Format
{
public:
    virtual ~DefaultFormat() override { }

    virtual QStringList shortDateFormats() override
    {
        return { m_locale.dateFormat(QLocale::ShortFormat) };
    }

    virtual QStringList longDateFormats() override
    {
        return { m_locale.dateFormat(QLocale::LongFormat) };
    }

    virtual QStringList shortTimeFormats() override
    {
        return { m_locale.timeFormat(QLocale::ShortFormat) };
    }

    virtual QStringList longTimeFormats() override
    {
        return { m_locale.timeFormat(QLocale::LongFormat) };
    }
};

class ChineseSimpliedFormat : public Format
{
public:
    virtual QStringList shortDateFormats() override
    {
        return { "yyyy/M/d",   "yyyy-M-d", "yyyy.M.d", "yyyy/MM/dd", "yyyy-MM-dd",
                 "yyyy.MM.dd", "yy/M/d",   "yy-M-d",   "yy.M.d" };
    }

    virtual QStringList longDateFormats() override
    {
        return { "yyyy年M月d日", "yyyy年M月d日，dddd", "yyyy年M月d日 dddd", "dddd yyyy年M月d日" };
    }

    virtual QStringList shortTimeFormats() override
    {
        return { "H:mm", "HH:mm", "AP h:mm", "AP hh:mm" };
    }

    virtual QStringList longTimeFormats() override
    {
        return { "H:mm:ss", "HH:mm:ss", "AP h:mm:ss", "AP hh:mm:ss" };
    }
};

class UKFormat : public Format
{
public:
    virtual QStringList shortDateFormats() override
    {
        return { "dd/MM/yyyy", "dd/MM/yy", "d/M/yy", "d.M.yy", "yyyy-MM-dd" };
    }

    virtual QStringList longDateFormats() override
    {
        return { "dd MMMM yyyy", "d MMMM yyyy", "dddd,d MMMM yyyy", "dddd, dd MMMM yyyy" };
    }

    virtual QStringList shortTimeFormats() override
    {
        return { "HH:mm", "H:mm", "hh:mm AP", "h:mm AP" };
    }

    virtual QStringList longTimeFormats() override
    {
        return { "HH:mm:ss", "H:mm:ss", "hh:mm:ss AP", "h:mm:ss AP" };
    }
};

class USAFormat : public Format
{
public:
    virtual QStringList shortDateFormats() override
    {
        return { "M/d/yyyy", "M/d/yy",     "MM/dd/yy", "MM/dd/yyyy",
                 "yy/MM/dd", "yyyy-MM-dd", "dd-MMM-yy" };
    }

    virtual QStringList longDateFormats() override
    {
        return { "dddd, MMMM d, yyyy", "MMMM d, yyyy", "dddd, d MMMM, yyyy", "d MMMM, yyyy" };
    }

    virtual QStringList shortTimeFormats() override
    {
        return { "h:mm Ap", "hh:mm Ap", "H:mm", "HH:mm" };
    }

    virtual QStringList longTimeFormats() override
    {
        return { "H:mm:ss ap", "HH:mm:ss ap", "H:mm:ss", "HH:mm:ss" };
    }
};

class WorldFormat : public Format
{
public:
    virtual QStringList shortDateFormats() override { return { "dd/MM/yyyy", "d MMM yyyy" }; }

    virtual QStringList longDateFormats() override
    {
        return { "dddd, d MMMM yyyy", "d MMMM yyyy" };
    }

    virtual QStringList shortTimeFormats() override { return { "H:mm AP", "HH:mm" }; }

    virtual QStringList longTimeFormats() override { return { "H:mm:ss AP", "HH:mm:ss" }; }
};

RegionAvailableData RegionProxy::m_formatData = RegionAvailableData();
RegionAvailableData RegionProxy::m_allFormat = RegionAvailableData();
RegionAvailableData RegionProxy::m_defaultFormat = RegionAvailableData();
RegionAvailableData RegionProxy::m_customFormat = RegionAvailableData();

RegionProxy::RegionProxy(QObject *parent)
    : QObject(parent)
    , m_translatorLanguage(nullptr)
    , m_translatorCountry(nullptr)
    , m_isActive(false)
{
}

// the locale even has sichuangYi. too many languages
void RegionProxy::active()
{
    if (m_isActive) {
        return;
    }
    m_isActive = true;
    m_translatorLanguage = new QTranslator(this);
    if (m_translatorLanguage->load(QLocale(), "datetime_language","_", TRANSLATE_READ_DIR)) {
        qApp->installTranslator(m_translatorLanguage);
    }
    m_translatorCountry = new QTranslator(this);
    if (m_translatorCountry->load(QLocale(), "datetime_country","_", TRANSLATE_READ_DIR)) {
        qApp->installTranslator(m_translatorCountry);
    }

    QList<QLocale> locales =
            QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    locales.removeOne(QLocale::C);
    // NOTE: sorry for Sichuang friends
    locales.removeOne(QLocale::SichuanYi);

    QStringList countries;
    for (const auto &locale : locales) {
        QString script = locale.scriptToString(locale.script());
        QString language = locale.languageToString(locale.language());
        QString country = locale.territoryToString(locale.territory());
        // NOTE: sorry for guangdong friends
        if (locale.language() == QLocale::Cantonese && locale.language() == QLocale::Chinese) {
            continue;
        }
        // NOTE: the region `World` is weird
        if (locale.territory() == QLocale::Territory::World) {
            continue;
        }
        if ((locale.territory() == QLocale::HongKong || locale.territory() == QLocale::Taiwan)
            && locale.language() == QLocale::Chinese)
            language = "Traditional Chinese";
        if (locale.territory() == QLocale::China && locale.language() == QLocale::Chinese)
            language = "Simplified Chinese";
        QString langCountry = QString("%1:%2").arg(language).arg(country);
        if (!countries.contains(country)) {
            countries << country;
            m_countries << country;
        }
        if (!m_regions.contains(langCountry))
            m_regions.insert(langCountry, locale);
    }
}

RegionProxy::~RegionProxy() { }

Regions RegionProxy::regions() const
{
    return m_regions;
}

RegionFormat RegionProxy::systemRegionFormat()
{
    return regionFormat(QLocale::system());
}

QStringList RegionProxy::countries() const
{
    return m_countries;
}

QString RegionProxy::systemCountry() const
{
    QString country = QLocale::system().territoryToString(QLocale::system().territory());
    return country;
}

QString RegionProxy::langCountry() const
{
    QLocale locale = QLocale::system();
    QString language = locale.languageToString(locale.language());
    QString country = locale.territoryToString(locale.territory());
    if ((locale.territory() == QLocale::HongKong || locale.territory() == QLocale::Taiwan)
        && locale.language() == QLocale::Chinese)
        language = "Traditional Chinese";
    if (locale.territory() == QLocale::China && locale.language() == QLocale::Chinese)
        language = "Simplified Chinese";
    QString langCountry = QString("%1:%2").arg(language).arg(country);
    return langCountry;
}

static inline QString replaceSpace(const QString &space) {
    QString sp;
    for (const QChar &c : space) {
        sp.append(c.isSpace() ? QChar(' ') : c);
    }
    return sp;
}

RegionFormat RegionProxy::regionFormat(const QLocale &locale)
{
    RegionFormat regionFormat;
    regionFormat.firstDayOfWeekFormat = locale.firstDayOfWeek();
    regionFormat.shortDateFormat = locale.dateFormat(QLocale::ShortFormat);
    regionFormat.longDateFormat = locale.dateFormat(QLocale::LongFormat);
    regionFormat.shortTimeFormat = locale.timeFormat(QLocale::ShortFormat);
    regionFormat.longTimeFormat = locale.timeFormat(QLocale::LongFormat);
    regionFormat.currencyFormat = locale.currencySymbol(QLocale::CurrencySymbol);
    // 如果是货币符号空就用 ¥
    if (regionFormat.currencyFormat.isEmpty())
        regionFormat.currencyFormat = QString::fromLocal8Bit("¥");

    regionFormat.numberFormat = replaceSpace(locale.toString(123456789));
    regionFormat.digitgroupFormat = locale.groupSeparator();
    regionFormat.paperFormat = "A4";

    return regionFormat;
}

RegionAvailableData RegionProxy::allTextData(const QLocale &locale)
{
    RegionAvailableData allTextData;
    allTextData += RegionProxy::defaultTextData(locale);
    allTextData += RegionProxy::customTextData(locale);

    m_allFormat.clear();
    m_allFormat += m_defaultFormat;
    m_allFormat += m_customFormat;

    return allTextData;
}

RegionAvailableData RegionProxy::allFormat()
{
    return m_allFormat;
}

RegionAvailableData RegionProxy::customTextData(const QLocale &locale)
{
    Format *format = nullptr;
    if (locale.territory() == QLocale::China && locale.script() == QLocale::SimplifiedHanScript) {
        format = new ChineseSimpliedFormat();
    } else if (locale.territory() == QLocale::UnitedKingdom
               && locale.language() == QLocale::English) {
        format = new UKFormat();
    } else if (locale.territory() == QLocale::UnitedStates && locale.language() == QLocale::English) {
        format = new USAFormat();
    } else if (locale.territory() == QLocale::World && locale.language() == QLocale::English) {
        format = new WorldFormat();
    } else {
        return RegionAvailableData();
    }

    QScopedPointer<Format> pFormat(format);
    pFormat->updateState(CurrentDate, CurrentTime, locale);

    RegionAvailableData textData;
    textData.daysAvailable = pFormat->daysText();
    textData.shortDatesAvailable = pFormat->shortDatesText();
    textData.longDatesAvailable = pFormat->longDatesText();
    textData.shortTimesAvailable = pFormat->shortTimesText();
    textData.longTimesAvailable = pFormat->longTimesText();

    m_customFormat.daysAvailable = pFormat->daysText(); // TODO format
    m_customFormat.shortDatesAvailable = pFormat->shortDateFormats();
    m_customFormat.longDatesAvailable = pFormat->longDateFormats();
    m_customFormat.shortTimesAvailable = pFormat->shortTimeFormats();
    m_customFormat.longTimesAvailable = pFormat->longTimeFormats();

    return textData;
}

RegionAvailableData RegionProxy::defaultTextData(const QLocale &locale)
{
    QScopedPointer<Format> defaultFormat(new DefaultFormat());
    defaultFormat->updateState(CurrentDate, CurrentTime, locale);

    RegionAvailableData textData;
    textData.daysAvailable = defaultFormat->daysText();
    textData.shortDatesAvailable = defaultFormat->shortDatesText();
    textData.longDatesAvailable = defaultFormat->longDatesText();
    textData.shortTimesAvailable = defaultFormat->shortTimesText();
    textData.longTimesAvailable = defaultFormat->longTimesText();

    m_defaultFormat.daysAvailable = defaultFormat->daysText(); // TODO format
    m_defaultFormat.shortDatesAvailable = defaultFormat->shortDateFormats();
    m_defaultFormat.longDatesAvailable = defaultFormat->longDateFormats();
    m_defaultFormat.shortTimesAvailable = defaultFormat->shortTimeFormats();
    m_defaultFormat.longTimesAvailable = defaultFormat->longTimeFormats();

    return textData;
}

QDebug operator<<(QDebug debug, const RegionFormat &regionFormat)
{
    debug << regionFormat.firstDayOfWeekFormat << regionFormat.shortDateFormat
          << regionFormat.longDateFormat << regionFormat.shortTimeFormat
          << regionFormat.longTimeFormat << regionFormat.currencyFormat << regionFormat.numberFormat
          << regionFormat.digitgroupFormat << regionFormat.paperFormat;

    return debug;
}
