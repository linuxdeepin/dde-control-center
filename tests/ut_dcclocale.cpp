// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dcclocale.h"

#include <gtest/gtest.h>

#include <QString>
#include <QCoreApplication>
#include <QPair>

// DCCLocale::stripTimezoneFromTimeFormat strips Qt time-format timezone tokens
// (the bare `t`/`T` run and its half/full-width wrapped variants) while keeping
// locale-specific quoted literals and their trailing punctuation. It is a pure
// string transform with no QCoreApplication/ICU runtime dependency, so the
// expectations below are fully deterministic.

namespace {

QString stripped(const QString &format)
{
    return DCCLocale::stripTimezoneFromTimeFormat(format);
}

} // namespace

TEST(DCCLocaleStripTimezone, EmptyInputIsUnchanged)
{
    EXPECT_EQ(stripped(QString()), QString());
}

TEST(DCCLocaleStripTimezone, FormatWithoutTimezoneTokenIsUnchanged)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss")), QStringLiteral("HH:mm:ss"));
    EXPECT_EQ(stripped(QStringLiteral("yyyy-MM-dd")), QStringLiteral("yyyy-MM-dd"));
}

TEST(DCCLocaleStripTimezone, QuotedLiteralsArePreserved)
{
    // fr_CA style literal — no timezone token, must survive verbatim.
    EXPECT_EQ(stripped(QStringLiteral("'h'/'min'/'s'")), QStringLiteral("'h'/'min'/'s'"));
}

TEST(DCCLocaleStripTimezone, HalfWidthParenWrappedTokenIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss (tttt)")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, FullWidthParenWrappedTokenIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss（tttt）")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, SquareBracketWrappedTokenIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm [tttt]")), QStringLiteral("HH:mm"));
}

TEST(DCCLocaleStripTimezone, BareTokenWithPrecedingSpaceIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss tttt")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, BareTokenWithCommaSeparatorIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("H:mm:ss, tttt")), QStringLiteral("H:mm:ss"));
}

TEST(DCCLocaleStripTimezone, FullWidthSemicolonAndUppercaseTokenIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss；TTTT")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, MixedCaseBareTokenIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss TtTt")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, UppercaseTokenInParensIsRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("HH:mm:ss (TTTT)")), QStringLiteral("HH:mm:ss"));
}

TEST(DCCLocaleStripTimezone, OnlyTimezoneTokenCollapsesToEmpty)
{
    EXPECT_EQ(stripped(QStringLiteral("tttt")), QString());
}

TEST(DCCLocaleStripTimezone, MultipleWrappedTokensAreAllRemoved)
{
    EXPECT_EQ(stripped(QStringLiteral("(tttt) [tttt]")), QString());
}

TEST(DCCLocaleStripTimezone, TokenInMiddleKeepsSurroundingSeparators)
{
    EXPECT_EQ(stripped(QStringLiteral("yyyy tttt HH:mm")), QStringLiteral("yyyy HH:mm"));
}

TEST(DCCLocaleStripTimezone, PeriodIsNotConsumedAsLeadingSeparator)
{
    // A period directly before a token is not in the separator class, so the
    // token is removed but the period survives (the bg_BG "'ch'." guarantee).
    EXPECT_EQ(stripped(QStringLiteral("a.tttt")), QStringLiteral("a."));
}

TEST(DCCLocaleStripTimezone, QuotedLiteralWithPeriodAndTokenIsPreserved)
{
    // bg_BG-style "'x'." literal followed by a bare token: the literal and its
    // trailing period must survive, only the token (with its preceding space)
    // is removed.
    EXPECT_EQ(stripped(QStringLiteral("a 'x'. tttt")), QStringLiteral("a 'x'."));
}

// ---------------------------------------------------------------------------
// QCoreApplication setup for ICU/translate-dependent tests.
//
// DCCLocale::dialectNames and languageAndRegionName rely on
// QCoreApplication::translate (for the hardcoded zh_HK/zh_TW/nan_TW and TW
// region strings) and on the ICU runtime (icu::LocaleDisplayNames created from
// the default locale). The unit-test binary links gtest_main, which supplies
// main(), so we register a global test environment that constructs a
// QCoreApplication before any test runs and destroys it afterwards.
// AddGlobalTestEnvironment() runs during static initialization, i.e. before
// RUN_ALL_TESTS(), so SetUp() precedes every test in the binary.

class QCoreApplicationEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        if (!qApp) {
            static int argc = 1;
            static char arg0[] = "unit-test";
            static char *argv[] = {arg0, nullptr};
            m_app = new QCoreApplication(argc, argv);
        }
    }

    void TearDown() override
    {
        delete m_app;
        m_app = nullptr;
    }

private:
    QCoreApplication *m_app = nullptr;
};

::testing::Environment *const g_qtAppEnv =
    ::testing::AddGlobalTestEnvironment(new QCoreApplicationEnvironment);

// ---------------------------------------------------------------------------
// DCCLocale::dialectNames
//
// The zh_HK / zh_TW / nan_TW branches return hardcoded English strings via
// QCoreApplication::translate; without a translation catalog installed the
// source string is returned verbatim, so those branches are asserted exactly.
// The fallback branch queries ICU LocaleDisplayNames (ULDN_DIALECT_NAMES),
// whose output is rendered in the system default locale, so those cases assert
// a non-empty result and that the returned list length matches the input
// (reserve preallocation).

TEST(DCCLocaleDialectNames, EmptyListReturnsEmpty)
{
    EXPECT_TRUE(DCCLocale::dialectNames({}).isEmpty());
    EXPECT_EQ(DCCLocale::dialectNames({}).size(), 0);
}

TEST(DCCLocaleDialectNames, ZhHongKongBranch)
{
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("zh_HK")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first(), QStringLiteral("Traditional Chinese (Chinese Hong Kong)"));
}

TEST(DCCLocaleDialectNames, ZhHongKongPrefixAlsoMatches)
{
    // startsWith("zh_HK") is checked against the original code, so a code with a
    // trailing variant still hits the hardcoded branch.
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("zh_HK.UTF-8")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first(), QStringLiteral("Traditional Chinese (Chinese Hong Kong)"));
}

TEST(DCCLocaleDialectNames, ZhTaiwanBranch)
{
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("zh_TW")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first(), QStringLiteral("Traditional Chinese (Chinese Taiwan)"));
}

TEST(DCCLocaleDialectNames, ZhTaiwanBranchSurvivesLatinSuffix)
{
    // The @latin suffix is stripped only for the ICU fallback path; the special
    // branch keys off the original code, so zh_TW@latin still returns the
    // hardcoded string.
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("zh_TW@latin")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first(), QStringLiteral("Traditional Chinese (Chinese Taiwan)"));
}

TEST(DCCLocaleDialectNames, NanTaiwanBranch)
{
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("nan_TW")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first(), QStringLiteral("Min Nan Chinese"));
}

TEST(DCCLocaleDialectNames, LatinSuffixIsStrippedBeforeIcuQuery)
{
    // "sr@latin" and "sr" both feed "sr" to ICU after stripping, hence identical
    // and non-empty dialect names.
    const QStringList withSuffix = DCCLocale::dialectNames({QStringLiteral("sr@latin")});
    const QStringList withoutSuffix = DCCLocale::dialectNames({QStringLiteral("sr")});
    ASSERT_EQ(withSuffix.size(), 1);
    ASSERT_EQ(withoutSuffix.size(), 1);
    EXPECT_FALSE(withSuffix.first().isEmpty());
    EXPECT_EQ(withSuffix.first(), withoutSuffix.first());
}

TEST(DCCLocaleDialectNames, NormalLocaleUsesIcuQuery)
{
    const QStringList result = DCCLocale::dialectNames({QStringLiteral("en_US")});
    ASSERT_EQ(result.size(), 1);
    EXPECT_FALSE(result.first().isEmpty());
}

TEST(DCCLocaleDialectNames, EmptyCodeFallsThroughToIcuPath)
{
    // An empty string skips all special branches and reaches localeDisplayName.
    const QStringList result = DCCLocale::dialectNames({QString()});
    EXPECT_EQ(result.size(), 1);
    ASSERT_FALSE(result.first().isEmpty());
}

TEST(DCCLocaleDialectNames, MixedLocalesReturnOneEntryPerInput)
{
    const QStringList input{
        QStringLiteral("zh_HK"), QStringLiteral("en_US"),
        QStringLiteral("zh_TW"), QStringLiteral("fr_FR"),
        QStringLiteral("nan_TW"),
    };
    const QStringList result = DCCLocale::dialectNames(input);
    // reserve() only preallocates capacity; the observable contract is that the
    // result has exactly one entry per input code.
    ASSERT_EQ(result.size(), input.size());
    EXPECT_EQ(result.at(0), QStringLiteral("Traditional Chinese (Chinese Hong Kong)"));
    EXPECT_EQ(result.at(2), QStringLiteral("Traditional Chinese (Chinese Taiwan)"));
    EXPECT_EQ(result.at(4), QStringLiteral("Min Nan Chinese"));
    EXPECT_FALSE(result.at(1).isEmpty());
    EXPECT_FALSE(result.at(3).isEmpty());
}

// ---------------------------------------------------------------------------
// DCCLocale::languageAndRegionName
//
// Display language and country are rendered in the system default locale, so
// only the TW special branch (which overrides the country with a hardcoded
// string) is asserted exactly; the rest assert presence/absence of fields.

TEST(DCCLocaleLanguageAndRegionName, LocaleWithRegionHasLanguageAndCountry)
{
    const auto result = DCCLocale::languageAndRegionName(QStringLiteral("en_US"));
    EXPECT_FALSE(result.first.isEmpty());
    EXPECT_FALSE(result.second.isEmpty());
}

TEST(DCCLocaleLanguageAndRegionName, TaiwanRegionOverridesCountry)
{
    const auto result = DCCLocale::languageAndRegionName(QStringLiteral("zh_TW"));
    EXPECT_FALSE(result.first.isEmpty());
    EXPECT_EQ(result.second, QStringLiteral("Taiwan China"));
}

TEST(DCCLocaleLanguageAndRegionName, TaiwanOverrideIsIndependentOfLanguage)
{
    // region == "TW" triggers the override regardless of the language part.
    const auto result = DCCLocale::languageAndRegionName(QStringLiteral("nan_TW"));
    EXPECT_FALSE(result.first.isEmpty());
    EXPECT_EQ(result.second, QStringLiteral("Taiwan China"));
}

TEST(DCCLocaleLanguageAndRegionName, LocaleWithoutRegionHasEmptyCountry)
{
    const auto result = DCCLocale::languageAndRegionName(QStringLiteral("zh"));
    EXPECT_FALSE(result.first.isEmpty());
    EXPECT_TRUE(result.second.isEmpty());
}
