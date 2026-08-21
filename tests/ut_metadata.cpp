// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "metadata.h"

#include <gtest/gtest.h>

#include <QDebug>
#include <QString>

// dccV25::MetaData is a value object backing the keyboard layout list model.
// Its non-trivial behavior — the pinyin() fallback, equality keyed on text
// only, and ordering by raw pinyin — is exercised here.

namespace {

using dccV25::MetaData;

} // namespace

TEST(MetaData, ConstructorStoresTextAndSectionAndDefaultsOthers)
{
    const MetaData md(QStringLiteral("foo"), true);

    EXPECT_EQ(md.text(), QStringLiteral("foo"));
    EXPECT_TRUE(md.section());
    EXPECT_FALSE(md.selected());
    EXPECT_EQ(md.key(), QString());
    // pinyin() falls back to text until a pinyin is set.
    EXPECT_EQ(md.pinyin(), QStringLiteral("foo"));
}

TEST(MetaData, PinyinFallsBackToTextWhenEmpty)
{
    MetaData md(QStringLiteral("display"));
    EXPECT_EQ(md.pinyin(), QStringLiteral("display"));

    md.setPinyin(QStringLiteral(""));
    EXPECT_EQ(md.pinyin(), QStringLiteral("display"));

    md.setPinyin(QStringLiteral("pinyin"));
    EXPECT_EQ(md.pinyin(), QStringLiteral("pinyin"));
}

TEST(MetaData, AccessorsRoundTrip)
{
    MetaData md;
    md.setKey(QStringLiteral("k1"));
    md.setText(QStringLiteral("t1"));
    md.setSection(true);
    md.setSelected(true);

    EXPECT_EQ(md.key(), QStringLiteral("k1"));
    EXPECT_EQ(md.text(), QStringLiteral("t1"));
    EXPECT_TRUE(md.section());
    EXPECT_TRUE(md.selected());

    md.setSection(false);
    md.setSelected(false);
    EXPECT_FALSE(md.section());
    EXPECT_FALSE(md.selected());
}

TEST(MetaData, EqualityIsKeyedOnTextOnly)
{
    MetaData a(QStringLiteral("same"));
    a.setPinyin(QStringLiteral("p1"));
    a.setKey(QStringLiteral("k1"));

    MetaData b(QStringLiteral("same"));
    b.setPinyin(QStringLiteral("p2"));
    b.setKey(QStringLiteral("k2"));

    EXPECT_TRUE(a == b);

    MetaData c(QStringLiteral("different"));
    EXPECT_FALSE(a == c);
}

TEST(MetaData, GreaterThanOrdersByPinyinCaseInsensitively)
{
    MetaData a;
    a.setPinyin(QStringLiteral("b"));
    MetaData b;
    b.setPinyin(QStringLiteral("a"));

    EXPECT_TRUE(a > b);
    EXPECT_FALSE(b > a);

    // Case-insensitive: "B" should not sort after "a".
    MetaData upper;
    upper.setPinyin(QStringLiteral("B"));
    EXPECT_TRUE(upper > b);
    EXPECT_FALSE(b > upper);

    // Equal (ignoring case) is not greater-than in either direction.
    MetaData sameDifferentCase;
    sameDifferentCase.setPinyin(QStringLiteral("b"));
    EXPECT_FALSE(upper > sameDifferentCase);
    EXPECT_FALSE(sameDifferentCase > upper);
}

TEST(MetaData, GreaterThanUsesRawPinyinNotFallback)
{
    // operator> compares m_pinyin directly. With no pinyin set, both sides are
    // empty, so neither is greater — regardless of the displayed text.
    MetaData a(QStringLiteral("zzz"));
    MetaData b(QStringLiteral("aaa"));

    EXPECT_FALSE(a > b);
    EXPECT_FALSE(b > a);
}

TEST(MetaData, DebugOutputStreamsAllFields)
{
    MetaData md(QStringLiteral("display"));
    md.setKey(QStringLiteral("k1"));
    md.setPinyin(QStringLiteral("py1"));
    md.setSection(true);
    md.setSelected(true);

    QString captured;
    {
        QDebug dbg(&captured);
        dbg << md;
    }

    // operator<< formats as "key: %1, text: %2, m_section: %3, pinyin: %4"
    // via QString::arg. Note: QString has no arg(bool) overload, so
    // .arg(md.section()) resolves to arg(int) — true renders as "1".
    EXPECT_TRUE(captured.contains(QStringLiteral("k1")));
    EXPECT_TRUE(captured.contains(QStringLiteral("display")));
    EXPECT_TRUE(captured.contains(QStringLiteral("py1")));
    EXPECT_TRUE(captured.contains(QStringLiteral("m_section: 1")));  // section=true→int 1
}

TEST(MetaData, DebugOutputDoesNotCrashOnDefaultConstructed)
{
    // Default-constructed MetaData has empty key/text/pinyin and section=false;
    // operator<< must handle it without crashing (exercises QString::arg with
    // empty/zero values).
    MetaData md;

    QString captured;
    {
        QDebug dbg(&captured);
        dbg << md;
    }

    EXPECT_FALSE(captured.isNull());
}
