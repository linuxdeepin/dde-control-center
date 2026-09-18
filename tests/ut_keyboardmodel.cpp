// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardmodel.h"
#include "metadata.h"

#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QStringList>
#include <QMap>
#include <QList>

// dccV25::KeyboardModel is a QObject backing the keyboard layout module.
// It owns the layout list, language list, and various toggle/interval state
// with guarded setters that emit Q_SIGNALS only on real change. This file
// exercises every public/protected slot, the private convertLang helper
// (exposed via the protected=public macro), and the boundary/early-return
// branches.

using dccV25::KeyboardModel;
using dccV25::MetaData;

// QSignalSpy on langChanged(QList<MetaData>) needs QList<MetaData> registered
// as a metatype at runtime. Q_DECLARE_METATYPE(dccV25::MetaData) is in
// metadata.h; QList<MetaData> is auto-declared in Qt6 but explicit runtime
// registration avoids queued-connection copy failures.
namespace {
struct MetaTypeRegistration {
    MetaTypeRegistration() { qRegisterMetaType<QList<MetaData>>("QList<MetaData>"); }
};
const MetaTypeRegistration g_registerMetaType;
} // namespace

namespace {

MetaData makeMeta(const QString &key, const QString &text)
{
    MetaData md;
    md.setKey(key);
    md.setText(text);
    return md;
}

QList<MetaData> makeList(std::initializer_list<std::pair<QString, QString>> items)
{
    QList<MetaData> list;
    for (const auto &it : items) {
        list << makeMeta(it.first, it.second);
    }
    return list;
}

} // namespace

// ---- Constructor defaults ----

TEST(KeyboardModel, ConstructorDefaults)
{
    KeyboardModel m;
    EXPECT_TRUE(m.keyboardEnabled());
    EXPECT_TRUE(m.capsLock());
    EXPECT_TRUE(m.numLock());
    EXPECT_EQ(m.repeatInterval(), 1u);
    EXPECT_EQ(m.repeatDelay(), 1u);
    EXPECT_EQ(m.getLangChangedState(), 0);
    EXPECT_TRUE(m.curLayout().isEmpty());
    EXPECT_TRUE(m.curLang().isEmpty());
    EXPECT_TRUE(m.localLang().isEmpty());
    EXPECT_TRUE(m.langLists().isEmpty());
    EXPECT_TRUE(m.allShortcut().isEmpty());
    EXPECT_TRUE(m.kbLayout().isEmpty());
    EXPECT_TRUE(m.allLayout().isEmpty());
    EXPECT_TRUE(m.userLayout().isEmpty());
    EXPECT_TRUE(m.getUserLayoutList().isEmpty());
}

// ---- Layout lists ----

TEST(KeyboardModel, SetLayoutListsStoresData)
{
    KeyboardModel m;
    QMap<QString, QString> layouts;
    layouts.insert(QStringLiteral("us"), QStringLiteral("English (US)"));
    layouts.insert(QStringLiteral("fr"), QStringLiteral("French"));
    m.setLayoutLists(layouts);
    EXPECT_EQ(m.kbLayout(), layouts);
    EXPECT_EQ(m.kbLayout().size(), 2);
}

TEST(KeyboardModel, SetAllLayoutListsStoresData)
{
    KeyboardModel m;
    QMap<QString, QString> all;
    all.insert(QStringLiteral("us"), QStringLiteral("English (US)"));
    all.insert(QStringLiteral("de"), QStringLiteral("German"));
    all.insert(QStringLiteral("jp"), QStringLiteral("Japanese"));
    m.setAllLayoutLists(all);
    EXPECT_EQ(m.allLayout(), all);
    EXPECT_EQ(m.allLayout().size(), 3);
}

// ---- setLayout / curLayout ----

TEST(KeyboardModel, SetLayoutEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::curLayoutChanged);

    // Empty key → early return, no signal.
    m.setLayout(QString());
    EXPECT_EQ(spy.count(), 0);

    m.setLayout(QStringLiteral("us"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("us"));
    EXPECT_EQ(m.curLayout(), QStringLiteral("us"));

    // Same key → early return.
    m.setLayout(QStringLiteral("us"));
    EXPECT_EQ(spy.count(), 0);
    EXPECT_EQ(m.curLayout(), QStringLiteral("us"));

    // Different key → signal.
    m.setLayout(QStringLiteral("fr"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(m.curLayout(), QStringLiteral("fr"));
}

// ---- langByKey / langFromText / langLists ----

TEST(KeyboardModel, LangByKeyReturnsTextWhenFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}}));
    EXPECT_EQ(m.langByKey(QStringLiteral("us")), QStringLiteral("English"));
    EXPECT_EQ(m.langByKey(QStringLiteral("fr")), QStringLiteral("French"));
}

TEST(KeyboardModel, LangByKeyReturnsEmptyWhenNotFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    EXPECT_EQ(m.langByKey(QStringLiteral("xx")), QString());
    EXPECT_EQ(m.langByKey(QString()), QString());
}

TEST(KeyboardModel, LangByKeyReturnsEmptyWhenListEmpty)
{
    KeyboardModel m;
    EXPECT_EQ(m.langByKey(QStringLiteral("us")), QString());
}

TEST(KeyboardModel, LangFromTextReturnsKeyWhenFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}}));
    EXPECT_EQ(m.langFromText(QStringLiteral("English")), QStringLiteral("us"));
    EXPECT_EQ(m.langFromText(QStringLiteral("French")), QStringLiteral("fr"));
}

TEST(KeyboardModel, LangFromTextReturnsEmptyWhenNotFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    EXPECT_EQ(m.langFromText(QStringLiteral("German")), QString());
    EXPECT_EQ(m.langFromText(QString()), QString());
}

// ---- setLang / curLang ----

TEST(KeyboardModel, SetLangEmitsCurLangChangedWhenLangFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);

    m.setLang(QStringLiteral("us"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("English"));
    EXPECT_EQ(m.curLang(), QStringLiteral("English"));
}

TEST(KeyboardModel, SetLangDoesNotEmitWhenLangNotFound)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);

    // key not in list → langByKey returns empty → no emission.
    m.setLang(QStringLiteral("xx"));
    EXPECT_EQ(spy.count(), 0);
    EXPECT_TRUE(m.curLang().isEmpty());
}

TEST(KeyboardModel, SetLangDoesNotEmitWhenValueEmpty)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);

    m.setLang(QString());
    EXPECT_EQ(spy.count(), 0);
}

TEST(KeyboardModel, SetLangDoesNotEmitOnSameValue)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    m.setLang(QStringLiteral("us"));

    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);
    m.setLang(QStringLiteral("us"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- setLocaleLang / localLang (convertLang private helper) ----

TEST(KeyboardModel, SetLocaleLangConvertsKeysAndEmits)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}, {"de", "German"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLocalLangChanged);

    m.setLocaleLang({QStringLiteral("us"), QStringLiteral("fr")});
    ASSERT_EQ(spy.count(), 1);
    QStringList result = spy.takeFirst().at(0).toStringList();
    EXPECT_EQ(result, QStringList({QStringLiteral("English"), QStringLiteral("French")}));
    EXPECT_EQ(m.localLang(), result);
}

TEST(KeyboardModel, SetLocaleLangSkipsUnknownKeys)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLocalLangChanged);

    // "xx" not in list → skipped; "us" converted.
    m.setLocaleLang({QStringLiteral("xx"), QStringLiteral("us")});
    ASSERT_EQ(spy.count(), 1);
    QStringList result = spy.takeFirst().at(0).toStringList();
    EXPECT_EQ(result, QStringList({QStringLiteral("English")}));
}

TEST(KeyboardModel, SetLocaleLangDoesNotEmitWhenResultUnchanged)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    m.setLocaleLang({QStringLiteral("us")});

    QSignalSpy spy(&m, &KeyboardModel::curLocalLangChanged);
    m.setLocaleLang({QStringLiteral("us")});
    EXPECT_EQ(spy.count(), 0);
}

TEST(KeyboardModel, SetLocaleLangDoesNotEmitWhenResultEmpty)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    QSignalSpy spy(&m, &KeyboardModel::curLocalLangChanged);

    // All keys unknown → convertLang returns empty → no emission.
    m.setLocaleLang({QStringLiteral("xx"), QStringLiteral("yy")});
    EXPECT_EQ(spy.count(), 0);
    EXPECT_TRUE(m.localLang().isEmpty());
}

TEST(KeyboardModel, SetLocaleLangDoesNotEmitWhenAllKeysUnknownAndPreviousEmpty)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::curLocalLangChanged);

    // No lang list set, so all lookups return empty.
    m.setLocaleLang({QStringLiteral("us")});
    EXPECT_EQ(spy.count(), 0);
}

// ---- setLocaleList / langLists ----

TEST(KeyboardModel, SetLocaleListEmitsLangChanged)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::langChanged);

    auto list = makeList({{"us", "English"}, {"fr", "French"}});
    m.setLocaleList(list);
    ASSERT_EQ(spy.count(), 1);
    QList<MetaData> emitted = spy.takeFirst().at(0).value<QList<MetaData>>();
    EXPECT_EQ(emitted.size(), 2);
    EXPECT_EQ(m.langLists().size(), 2);
}

TEST(KeyboardModel, SetLocaleListDoesNotEmitOnEmpty)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::langChanged);

    m.setLocaleList({});
    EXPECT_EQ(spy.count(), 0);
    EXPECT_TRUE(m.langLists().isEmpty());
}

TEST(KeyboardModel, SetLocaleListEmitsCurLangIfKeySet)
{
    KeyboardModel m;
    m.setLang(QStringLiteral("us"));
    m.setLocaleList({});

    // Now set a list containing the current lang key → should emit curLangChanged.
    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);
    m.setLocaleList(makeList({{"us", "English"}}));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("English"));
}

TEST(KeyboardModel, SetLocaleListDoesNotEmitCurLangIfKeyNotFound)
{
    KeyboardModel m;
    m.setLang(QStringLiteral("us"));
    m.setLocaleList({});

    QSignalSpy spy(&m, &KeyboardModel::curLangChanged);
    m.setLocaleList(makeList({{"fr", "French"}}));
    EXPECT_EQ(spy.count(), 0);
}

// ---- setLangChangedState / onSetCurLangFinish ----

TEST(KeyboardModel, SetLangChangedStateEmitsOnRealChange)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::onSetCurLangFinish);

    m.setLangChangedState(1);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toInt(), 1);
    EXPECT_EQ(m.getLangChangedState(), 1);

    // Same value → no signal.
    m.setLangChangedState(1);
    EXPECT_EQ(spy.count(), 0);

    m.setLangChangedState(2);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(m.getLangChangedState(), 2);
}

// ---- setCapsLock ----

TEST(KeyboardModel, SetCapsLockEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    EXPECT_TRUE(m.capsLock());
    QSignalSpy spy(&m, &KeyboardModel::capsLockChanged);

    // Same → no signal.
    m.setCapsLock(true);
    EXPECT_EQ(spy.count(), 0);

    m.setCapsLock(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(m.capsLock());

    m.setCapsLock(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(m.capsLock());
}

// ---- setNumLock ----

TEST(KeyboardModel, SetNumLockEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    EXPECT_TRUE(m.numLock());
    QSignalSpy spy(&m, &KeyboardModel::numLockChanged);

    m.setNumLock(true);
    EXPECT_EQ(spy.count(), 0);

    m.setNumLock(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(m.numLock());

    m.setNumLock(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(m.numLock());
}

// ---- repeatDelay ----

TEST(KeyboardModel, SetRepeatDelayEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    EXPECT_EQ(m.repeatDelay(), 1u);
    QSignalSpy spy(&m, &KeyboardModel::repeatDelayChanged);

    m.setRepeatDelay(1);
    EXPECT_EQ(spy.count(), 0);

    m.setRepeatDelay(30);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toUInt(), 30u);
    EXPECT_EQ(m.repeatDelay(), 30u);
}

// ---- repeatInterval ----

TEST(KeyboardModel, SetRepeatIntervalEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    EXPECT_EQ(m.repeatInterval(), 1u);
    QSignalSpy spy(&m, &KeyboardModel::repeatIntervalChanged);

    m.setRepeatInterval(1);
    EXPECT_EQ(spy.count(), 0);

    m.setRepeatInterval(50);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toUInt(), 50u);
    EXPECT_EQ(m.repeatInterval(), 50u);
}

// ---- keyboardEnabled ----

TEST(KeyboardModel, SetKeyboardEnabledEmitsOnlyOnRealChange)
{
    KeyboardModel m;
    EXPECT_TRUE(m.keyboardEnabled());
    QSignalSpy spy(&m, &KeyboardModel::keyboardEnabledChanged);

    m.setKeyboardEnabled(true);
    EXPECT_EQ(spy.count(), 0);

    m.setKeyboardEnabled(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(m.keyboardEnabled());

    m.setKeyboardEnabled(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(m.keyboardEnabled());
}

// ---- addUserLayout / userLayout / cleanUserLayout ----

TEST(KeyboardModel, AddUserLayoutInsertsAndEmitsOnlyWhenNew)
{
    KeyboardModel m;
    QSignalSpy spy(&m, &KeyboardModel::userLayoutChanged);

    m.addUserLayout(QStringLiteral("us"), QStringLiteral("English (US)"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("us"));
    EXPECT_EQ(m.userLayout().value(QStringLiteral("us")), QStringLiteral("English (US)"));

    // Duplicate id → no signal, no insertion.
    m.addUserLayout(QStringLiteral("us"), QStringLiteral("Other"));
    EXPECT_EQ(spy.count(), 0);
    EXPECT_EQ(m.userLayout().value(QStringLiteral("us")), QStringLiteral("English (US)"));

    m.addUserLayout(QStringLiteral("fr"), QStringLiteral("French"));
    EXPECT_EQ(m.userLayout().size(), 2);
}

TEST(KeyboardModel, CleanUserLayoutClearsMap)
{
    KeyboardModel m;
    m.addUserLayout(QStringLiteral("us"), QStringLiteral("English"));
    m.addUserLayout(QStringLiteral("fr"), QStringLiteral("French"));
    ASSERT_EQ(m.userLayout().size(), 2);

    m.cleanUserLayout();
    EXPECT_TRUE(m.userLayout().isEmpty());
}

// ---- setAllShortcut / allShortcut ----

TEST(KeyboardModel, SetAllShortcutStoresMap)
{
    KeyboardModel m;
    QMap<QStringList, int> shortcuts;
    shortcuts.insert({QStringLiteral("a"), QStringLiteral("b")}, 1);
    shortcuts.insert({QStringLiteral("c")}, 2);
    m.setAllShortcut(shortcuts);
    EXPECT_EQ(m.allShortcut(), shortcuts);
    EXPECT_EQ(m.allShortcut().size(), 2);
}

// ---- convertLang (private, exposed via protected=public macro) ----

TEST(KeyboardModel, ConvertLangReturnsTextsForKnownKeys)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}, {"fr", "French"}}));
    QStringList result = m.convertLang({QStringLiteral("us"), QStringLiteral("fr")});
    EXPECT_EQ(result, QStringList({QStringLiteral("English"), QStringLiteral("French")}));
}

TEST(KeyboardModel, ConvertLangSkipsUnknownKeys)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    QStringList result = m.convertLang({QStringLiteral("us"), QStringLiteral("xx")});
    EXPECT_EQ(result, QStringList({QStringLiteral("English")}));
}

TEST(KeyboardModel, ConvertLangReturnsEmptyForEmptyInput)
{
    KeyboardModel m;
    m.setLocaleList(makeList({{"us", "English"}}));
    QStringList result = m.convertLang({});
    EXPECT_TRUE(result.isEmpty());
}

TEST(KeyboardModel, ConvertLangReturnsEmptyWhenNoLangList)
{
    KeyboardModel m;
    QStringList result = m.convertLang({QStringLiteral("us"), QStringLiteral("fr")});
    EXPECT_TRUE(result.isEmpty());
}
