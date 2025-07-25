//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "dcclocale.h"

#include <QCoreApplication>
#include <QLocale>

#include <unicode/locdspnm.h>

using namespace icu;

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

QStringList DCCLocale::dialectNames(const QStringList &localeCodes)
{
    QStringList results;

    icu::Locale currentLocale(icu::Locale::getDefault());
    icu::LocaleDisplayNames * displayNames = icu::LocaleDisplayNames::createInstance(currentLocale, ULDN_DIALECT_NAMES);
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
        displayNames->localeDisplayName(localeCodeWithoutSuffix.toStdString().c_str(), dialectName);
        results.append(toQString(dialectName));
    }
    return results;
}