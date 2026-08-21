//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "dcclocale.h"

#include <QCoreApplication>
#include <QGlobalStatic>
#include <QLocale>
#include <QRegularExpression>
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

    [[maybe_unused]] icu::UnicodeString fromQString(const QString& qstr) {
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
        } else if (localeCode.startsWith("nan_TW")) {
            results.append(QCoreApplication::translate("dcc::Locale::dialectNames", "Min Nan Chinese"));
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

// 两个正则都需要 static QRegularExpression，避免每次调用重新编译。
// 正则对象本身是线程安全的（只读使用），与 QRegularExpression 的文档一致。
static const QRegularExpression &tzTokenRegex()
{
    static const QRegularExpression re(
        // 1) 半角/全角括号、方括号整体包裹的时区: (tttt)、（tttt）、[tttt]
        // 2) 裸时区 token，连同前导分隔符(空格/逗号/分号，但不含句点)一并删除
        //    * 让前导分支也能匹配 zh_CN "tttt HH:mm:ss" 这种开头裸时区
        // 分隔符字符类不含句点，避免误吞 bg_BG "'ч'." 这种以缩写句点结尾的字面量；
        // 同样不匹配引号字面量本身，bg_BG 的 'ч'. 与 fr_CA 的 'h'/'min'/'s'
        // 都交由 Qt 的 toString 自行渲染，保证它们不会被剥离。
        "(?:\\([tT]+\\)|（[tT]+）|\\[[tT]+\\])"
        "|[\\s,，;；]*[tT]+"
    );
    return re;
}

static const QRegularExpression &trailingSepRegex()
{
    static const QRegularExpression re("[\\s,，;；]+\\s*$");
    return re;
}

QString DCCLocale::stripTimezoneFromTimeFormat(const QString &timeFormat)
{
    QString s = timeFormat;
    s.remove(tzTokenRegex());
    s.remove(trailingSepRegex());
    return s;
}
