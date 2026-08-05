// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pinyinsearch.h"

#include <gtest/gtest.h>

namespace {

using dccV25::buildPinyinSearchQuery;

TEST(PinyinSearch, matchesMultipleAlternativeReadings)
{
    const dccV25::PinyinSearchIndex index =
            dccV25::buildPinyinSearchIndex(QStringLiteral("度重"));

    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(QStringLiteral("duochong"))));
    EXPECT_FALSE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(QStringLiteral("duoxiang"))));

    dccV25::PinyinSearchIndex syntheticIndex;
    syntheticIndex.syllables = {{QStringLiteral("a"), QStringLiteral("x")},
                                {QStringLiteral("b"), QStringLiteral("y")},
                                {QStringLiteral("c"), QStringLiteral("z")}};
    EXPECT_TRUE(dccV25::matchesPinyin(syntheticIndex, buildPinyinSearchQuery(QStringLiteral("xyz"))));
}

TEST(PinyinSearch, preservesLaterPolyphonicReadings)
{
    const dccV25::PinyinSearchIndex index =
            dccV25::buildPinyinSearchIndex(QStringLiteral("度度度度度度重"));
    const QString allAlternatives = QStringLiteral("duo").repeated(6) + QStringLiteral("chong");

    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(allAlternatives)));
    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(QStringLiteral("chong"))));
}

TEST(PinyinSearch, matchesMultipleAlternativeInitials)
{
    dccV25::PinyinSearchIndex index;
    index.initials = {{QStringLiteral("a"), QStringLiteral("x")},
                      {QStringLiteral("b"), QStringLiteral("y")},
                      {QStringLiteral("c"), QStringLiteral("z")}};

    EXPECT_TRUE(dccV25::matchesPinyinInitials(index, buildPinyinSearchQuery(QStringLiteral("xyz"))));
    EXPECT_TRUE(dccV25::matchesPinyinInitials(index, buildPinyinSearchQuery(QStringLiteral("xy"))));
    EXPECT_FALSE(dccV25::matchesPinyinInitials(index, buildPinyinSearchQuery(QStringLiteral("xya"))));
}

TEST(PinyinSearch, matchesSubstringAcrossSyllables)
{
    dccV25::PinyinSearchIndex index;
    index.syllables = {{QStringLiteral("duo")}, {QStringLiteral("chong")}};

    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(QStringLiteral("och"))));
    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(QStringLiteral("CHONG"))));
}

TEST(PinyinSearch, compilesQueryOnceForFullAndInitialMatches)
{
    dccV25::PinyinSearchIndex index;
    index.syllables = {{QStringLiteral("shen")}, {QStringLiteral("du"), QStringLiteral("duo")}};
    index.initials = {{QStringLiteral("s")}, {QStringLiteral("d")}};
    const dccV25::PinyinSearchQuery fullQuery = buildPinyinSearchQuery(QStringLiteral("SHENDUO"));
    const dccV25::PinyinSearchQuery initialsQuery = buildPinyinSearchQuery(QStringLiteral("SD"));

    EXPECT_EQ(fullQuery.pattern, QStringLiteral("shenduo"));
    EXPECT_EQ(fullQuery.prefixTable.size(), fullQuery.pattern.size());
    EXPECT_TRUE(dccV25::matchesPinyinSearch(index, fullQuery));
    EXPECT_TRUE(dccV25::matchesPinyinSearch(index, initialsQuery));
}

TEST(PinyinSearch, storesLongPolyphonicTextLinearly)
{
    const QString text = QStringLiteral("深度影院").repeated(24);
    const QString alternativePinyin = QStringLiteral("shenduoyingyuan").repeated(24);
    const dccV25::PinyinSearchIndex index = dccV25::buildPinyinSearchIndex(text);

    qsizetype spellingCount = 0;
    qsizetype initialCount = 0;
    for (const QStringList &choices : index.syllables)
        spellingCount += choices.size();
    for (const QStringList &choices : index.initials)
        initialCount += choices.size();

    EXPECT_EQ(index.syllables.size(), text.size());
    EXPECT_EQ(index.initials.size(), text.size());
    EXPECT_LE(spellingCount, text.size() * 5);
    EXPECT_LE(initialCount, text.size() * 5);
    EXPECT_TRUE(dccV25::matchesPinyin(index, buildPinyinSearchQuery(alternativePinyin)));
    EXPECT_TRUE(dccV25::matchesPinyinInitials(
            index, buildPinyinSearchQuery(QStringLiteral("sdyy").repeated(24))));
}

}
