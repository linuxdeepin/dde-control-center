//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "dcclocale.h"

#include <QCoreApplication>
#include <QGlobalStatic>
#include <QLocale>
#include <memory>

#include <unicode/locdspnm.h>

using namespace icu;
using namespace Qt::Literals::StringLiterals;

namespace {
    // Cache LocaleDisplayNames instance
    struct DisplayNamesHolder {
        DisplayNamesHolder() 
            : displayNames(icu::LocaleDisplayNames::createInstance(
                icu::Locale::getDefault(), ULDN_DIALECT_NAMES)) {}
        
        std::unique_ptr<icu::LocaleDisplayNames> displayNames;
    };
    
    Q_GLOBAL_STATIC(DisplayNamesHolder, globalDisplayNames);
    
    icu::LocaleDisplayNames* getDisplayNames() {
        return globalDisplayNames()->displayNames.get();
    }

    icu::UnicodeString fromQString(const QString& qstr) {
        return icu::UnicodeString(qstr.utf16(), qstr.length());
    }
    
    QString toQString(const icu::UnicodeString& icuString) {
        // Get a pointer to the internal UTF-16 buffer of the icu::UnicodeString.
        // The buffer is not necessarily null-terminated, so we also need the length.
        const UChar* ucharData = icuString.getBuffer();
        int32_t length = icuString.length();
    
        // QString has a constructor that takes a const QChar* and a length.
        // UChar is typically a 16-bit unsigned integer, which is compatible with QChar.
        // Static_cast is used here for explicit type conversion, though often
        // UChar and QChar are typedefs to the same underlying type (e.g., unsigned short).
        return QString(reinterpret_cast<const QChar*>(ucharData), length);
    }
}

QStringList DCCLocale::dialectNames(const QStringList &localeCodes)
{
    QStringList results;
    results.reserve(localeCodes.size()); // 预分配空间

    icu::LocaleDisplayNames* displayNames = getDisplayNames();
    
    for (const QString &localeCode : std::as_const(localeCodes)) {
        // locale code might contain something like @latin, we need to remove such suffix
        QString localeCodeWithoutSuffix = localeCode.split("@").first();
        if (localeCode.startsWith("zh_HK")) {
            results.append(QCoreApplication::translate("dcc::Locale::dialectNames", "Traditional Chinese (Chinese Hong Kong)"));
            continue;
        } else if (localeCode.startsWith("zh_TW")) {
            results.append(QCoreApplication::translate("dcc::Locale::dialectNames", "Traditional Chinese (Chinese Taiwan)"));
            continue;
        }

        icu::UnicodeString dialectName;
        displayNames->localeDisplayName(localeCodeWithoutSuffix.toLatin1().constData(), dialectName);
        results.append(toQString(dialectName));
    }
    return results;
}

QPair<QString, QString> DCCLocale::languageAndRegionName(const QString &localeCode)
{
    const icu::Locale& systemLocale = icu::Locale::getDefault();
    icu::UnicodeString localeUString;
    icu::Locale icuLocale(localeCode.toLatin1().constData());
    auto regionCode = QString(icuLocale.getCountry());
    QString displayLanguage = toQString(icuLocale.getDisplayLanguage(systemLocale, localeUString));
    QString displayCountry = toQString(icuLocale.getDisplayCountry(systemLocale, localeUString));

    if (regionCode == u"TW"_s) {
        displayCountry = QCoreApplication::translate("dcc::Locale::regionNames", "Taiwan China");
    }

    return { displayLanguage, displayCountry };
}
