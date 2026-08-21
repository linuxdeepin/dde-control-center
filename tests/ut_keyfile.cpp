// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "keyfile.h"

#include <gtest/gtest.h>

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QTextStream>

// ---- Constructor / Destructor ----

TEST(KeyFile, DefaultConstructorUsesSemicolonSeparator)
{
    KeyFile kf;
    // listSeparator defaults to ';'; verified indirectly via getStrList.
    kf.setKey("sec", "k", "a;b;c");
    EXPECT_EQ(kf.getStrList("sec", "k"), QStringList({"a", "b", "c"}));
}

TEST(KeyFile, CustomSeparator)
{
    KeyFile kf(',');
    kf.setKey("sec", "k", "a,b,c");
    EXPECT_EQ(kf.getStrList("sec", "k"), QStringList({"a", "b", "c"}));
}

// ---- getBool ----

TEST(KeyFile, GetBoolReturnsFalseForMissingSection)
{
    KeyFile kf;
    // Section not found → returns false (NOT defaultValue).
    EXPECT_FALSE(kf.getBool("nope", "k", true));
}

TEST(KeyFile, GetBoolTrue)
{
    KeyFile kf;
    kf.setKey("sec", "k", "true");
    EXPECT_TRUE(kf.getBool("sec", "k", false));
}

TEST(KeyFile, GetBoolFalse)
{
    KeyFile kf;
    kf.setKey("sec", "k", "false");
    EXPECT_FALSE(kf.getBool("sec", "k", true));
}

TEST(KeyFile, GetBoolNonBooleanValueReturnsDefaultValue)
{
    KeyFile kf;
    kf.setKey("sec", "k", "maybe");
    EXPECT_TRUE(kf.getBool("sec", "k", true));
    EXPECT_FALSE(kf.getBool("sec", "k", false));
}

TEST(KeyFile, GetBoolEmptyValueReturnsDefaultValue)
{
    KeyFile kf;
    kf.setKey("sec", "k", "");
    EXPECT_TRUE(kf.getBool("sec", "k", true));
}

TEST(KeyFile, GetBoolCaseSensitive)
{
    KeyFile kf;
    kf.setKey("sec", "k", "True");
    // Only exact "true"/"false" match; "True" falls through to defaultValue.
    EXPECT_FALSE(kf.getBool("sec", "k", false));
}

// ---- getStr ----

TEST(KeyFile, GetStrReturnsDefaultForMissingSection)
{
    KeyFile kf;
    EXPECT_EQ(kf.getStr("nope", "k", "def"), QStringLiteral("def"));
}

TEST(KeyFile, GetStrReturnsDefaultForEmptyValue)
{
    KeyFile kf;
    kf.setKey("sec", "k", "");
    EXPECT_EQ(kf.getStr("sec", "k", "def"), QStringLiteral("def"));
}

TEST(KeyFile, GetStrReturnsValue)
{
    KeyFile kf;
    kf.setKey("sec", "k", "hello");
    EXPECT_EQ(kf.getStr("sec", "k", "def"), QStringLiteral("hello"));
}

TEST(KeyFile, GetStrDefaultDefaultIsEmpty)
{
    KeyFile kf;
    // Missing section with default defaultValue param ("").
    EXPECT_EQ(kf.getStr("nope", "k"), QStringLiteral(""));
}

// ---- containKey ----

TEST(KeyFile, ContainKeyMissingSection)
{
    KeyFile kf;
    EXPECT_FALSE(kf.containKey("nope", "k"));
}

TEST(KeyFile, ContainKeyPresent)
{
    KeyFile kf;
    kf.setKey("sec", "k", "v");
    EXPECT_TRUE(kf.containKey("sec", "k"));
}

TEST(KeyFile, ContainKeyAbsent)
{
    KeyFile kf;
    kf.setKey("sec", "k1", "v");
    EXPECT_FALSE(kf.containKey("sec", "k2"));
}

// ---- getStrList ----

TEST(KeyFile, GetStrListSplitsBySeparator)
{
    KeyFile kf;
    kf.setKey("sec", "k", "a;b;c");
    EXPECT_EQ(kf.getStrList("sec", "k"), QStringList({"a", "b", "c"}));
}

TEST(KeyFile, GetStrListNoSeparatorReturnsSingleElement)
{
    KeyFile kf;
    kf.setKey("sec", "k", "single");
    EXPECT_EQ(kf.getStrList("sec", "k"), QStringList({"single"}));
}

TEST(KeyFile, GetStrListMissingSectionReturnsSingleEmpty)
{
    KeyFile kf;
    // getStr returns "" (default), "".split(';') → [""]
    EXPECT_EQ(kf.getStrList("nope", "k"), QStringList({QString()}));
}

// ---- setKey ----

TEST(KeyFile, SetKeyCreatesSection)
{
    KeyFile kf;
    kf.setKey("newsec", "k", "v");
    EXPECT_TRUE(kf.containKey("newsec", "k"));
    EXPECT_EQ(kf.getStr("newsec", "k"), QStringLiteral("v"));
}

TEST(KeyFile, SetKeyOverwritesExisting)
{
    KeyFile kf;
    kf.setKey("sec", "k", "old");
    kf.setKey("sec", "k", "new");
    EXPECT_EQ(kf.getStr("sec", "k"), QStringLiteral("new"));
}

TEST(KeyFile, SetKeyMultipleKeysInSameSection)
{
    KeyFile kf;
    kf.setKey("sec", "k1", "v1");
    kf.setKey("sec", "k2", "v2");
    EXPECT_EQ(kf.getStr("sec", "k1"), QStringLiteral("v1"));
    EXPECT_EQ(kf.getStr("sec", "k2"), QStringLiteral("v2"));
}

// ---- getMainKeys ----

TEST(KeyFile, GetMainKeysEmpty)
{
    KeyFile kf;
    EXPECT_TRUE(kf.getMainKeys().isEmpty());
}

TEST(KeyFile, GetMainKeysReturnsAllSections)
{
    KeyFile kf;
    kf.setKey("s1", "k", "v");
    kf.setKey("s2", "k", "v");
    kf.setKey("s3", "k", "v");
    QStringList keys = kf.getMainKeys();
    EXPECT_EQ(keys.size(), 3);
    EXPECT_TRUE(keys.contains("s1"));
    EXPECT_TRUE(keys.contains("s2"));
    EXPECT_TRUE(keys.contains("s3"));
}

// ---- removeSection / removeKey ----

TEST(KeyFile, RemoveSection)
{
    KeyFile kf;
    kf.setKey("s1", "k", "v");
    kf.setKey("s2", "k", "v");
    kf.removeSection("s1");
    EXPECT_FALSE(kf.containKey("s1", "k"));
    EXPECT_TRUE(kf.containKey("s2", "k"));
    EXPECT_EQ(kf.getMainKeys().size(), 1);
}

TEST(KeyFile, RemoveSectionNonExistentIsNoop)
{
    KeyFile kf;
    kf.setKey("s1", "k", "v");
    kf.removeSection("nope");
    EXPECT_EQ(kf.getMainKeys().size(), 1);
}

TEST(KeyFile, RemoveKey)
{
    KeyFile kf;
    kf.setKey("sec", "k1", "v1");
    kf.setKey("sec", "k2", "v2");
    kf.removeKey("sec", "k1");
    EXPECT_FALSE(kf.containKey("sec", "k1"));
    EXPECT_TRUE(kf.containKey("sec", "k2"));
}

TEST(KeyFile, RemoveKeyFromNonExistentSectionIsNoop)
{
    KeyFile kf;
    kf.removeKey("nope", "k");
    // No crash, no data added.
    EXPECT_TRUE(kf.getMainKeys().isEmpty());
}

TEST(KeyFile, RemoveKeyNonExistentKeyIsNoop)
{
    KeyFile kf;
    kf.setKey("sec", "k1", "v1");
    kf.removeKey("sec", "k2");
    EXPECT_TRUE(kf.containKey("sec", "k1"));
}

// ---- saveToFile / loadFile round-trip ----

TEST(KeyFile, SaveAndLoadRoundTrip)
{
    KeyFile kf;
    kf.setKey("Section1", "key1", "value1");
    kf.setKey("Section1", "key2", "value2");
    kf.setKey("Section2", "keyA", "valueA");

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_roundtrip_test.ini");
    ASSERT_TRUE(kf.saveToFile(path));

    KeyFile kf2;
    ASSERT_TRUE(kf2.loadFile(path));

    EXPECT_EQ(kf2.getStr("Section1", "key1"), QStringLiteral("value1"));
    EXPECT_EQ(kf2.getStr("Section1", "key2"), QStringLiteral("value2"));
    EXPECT_EQ(kf2.getStr("Section2", "keyA"), QStringLiteral("valueA"));
    EXPECT_EQ(kf2.getMainKeys().size(), 2);
}

TEST(KeyFile, SaveToFileOverwritesExistingFile)
{
    KeyFile kf1;
    kf1.setKey("S", "k", "first");

    KeyFile kf2;
    kf2.setKey("S", "k", "second");

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_overwrite_test.ini");

    ASSERT_TRUE(kf1.saveToFile(path));
    ASSERT_TRUE(kf2.saveToFile(path));

    KeyFile loader;
    ASSERT_TRUE(loader.loadFile(path));
    EXPECT_EQ(loader.getStr("S", "k"), QStringLiteral("second"));
}

// ---- loadFile parsing details ----

TEST(KeyFile, LoadFileParsesComments)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_comments_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "# This is a comment\n"
           << "[Section]\n"
           << "  # indented comment\n"
           << "key=value\n";
    }

    KeyFile kf;
    ASSERT_TRUE(kf.loadFile(path));
    EXPECT_EQ(kf.getStr("Section", "key"), QStringLiteral("value"));
    EXPECT_EQ(kf.getMainKeys().size(), 1);
}

TEST(KeyFile, LoadFileStripsLeadingSpaces)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_leading_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "   [Section]\n"
           << "   key=value\n";
    }

    KeyFile kf;
    ASSERT_TRUE(kf.loadFile(path));
    EXPECT_EQ(kf.getStr("Section", "key"), QStringLiteral("value"));
}

TEST(KeyFile, LoadFileSkipsLinesWithoutEquals)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_noeq_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "[Section]\n"
           << "this line has no equals sign\n"
           << "key=value\n";
    }

    KeyFile kf;
    ASSERT_TRUE(kf.loadFile(path));
    EXPECT_EQ(kf.getStr("Section", "key"), QStringLiteral("value"));
    // The no-equals line is skipped, not stored.
    EXPECT_FALSE(kf.containKey("Section", "this line has no equals sign"));
}

TEST(KeyFile, LoadFileReturnsFalseForKeyValueBeforeSection)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_kvbefore_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "key=value\n"
           << "[Section]\n";
    }

    KeyFile kf;
    EXPECT_FALSE(kf.loadFile(path));
}

TEST(KeyFile, LoadFileReturnsFalseForNonExistentFile)
{
    KeyFile kf;
    EXPECT_FALSE(kf.loadFile("/nonexistent/path/that/does/not/exist.ini"));
}

TEST(KeyFile, LoadFileClearsExistingData)
{
    KeyFile kf;
    kf.setKey("Existing", "k", "v");
    ASSERT_EQ(kf.getMainKeys().size(), 1);

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_clear_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "[NewSection]\n"
           << "nk=nv\n";
    }

    ASSERT_TRUE(kf.loadFile(path));
    // Old data cleared, only new data present.
    EXPECT_FALSE(kf.containKey("Existing", "k"));
    EXPECT_EQ(kf.getMainKeys().size(), 1);
    EXPECT_EQ(kf.getStr("NewSection", "nk"), QStringLiteral("nv"));
}

TEST(KeyFile, LoadFileParsesValueWithEqualsSign)
{
    // Value containing '=' should split only on the first '='.
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_eqval_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "[Section]\n"
           << "key=a=b=c\n";
    }

    KeyFile kf;
    ASSERT_TRUE(kf.loadFile(path));
    EXPECT_EQ(kf.getStr("Section", "key"), QStringLiteral("a=b=c"));
}

TEST(KeyFile, LoadFileMultipleSections)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    const QString path = tempDir.filePath("keyfile_multi_test.ini");
    {
        QFile f(path);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        ts << "[S1]\n"
           << "a=1\n"
           << "[S2]\n"
           << "b=2\n"
           << "[S3]\n"
           << "c=3\n";
    }

    KeyFile kf;
    ASSERT_TRUE(kf.loadFile(path));
    EXPECT_EQ(kf.getMainKeys().size(), 3);
    EXPECT_EQ(kf.getStr("S1", "a"), QStringLiteral("1"));
    EXPECT_EQ(kf.getStr("S2", "b"), QStringLiteral("2"));
    EXPECT_EQ(kf.getStr("S3", "c"), QStringLiteral("3"));
}

TEST(KeyFile, SaveToFileReturnsFalseForInvalidPath)
{
    KeyFile kf;
    kf.setKey("s", "k", "v");
    // Directory does not exist → cannot open for write.
    EXPECT_FALSE(kf.saveToFile("/nonexistent/dir/path/file.ini"));
}
