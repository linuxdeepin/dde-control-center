// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "category.h"

#include <gtest/gtest.h>

#include <QList>
#include <QObject>
#include <QSignalSpy>
#include <QMetaType>
#include <QtTest>

// App is a plain struct used as a Qt signal argument. QSignalSpy requires the
// type be declared as a metatype and registered before the first connection.
Q_DECLARE_METATYPE(App)

namespace {

// Register the App metatype once globally so QSignalSpy can capture it.
struct AppMetaTypeRegistration {
    AppMetaTypeRegistration() { qRegisterMetaType<App>("App"); }
};
const AppMetaTypeRegistration g_registerAppMetaType;

App makeApp(const QString &id, bool isUser, const QString &name = QString())
{
    App a;
    a.Id = id;
    a.Name = name;
    a.isUser = isUser;
    return a;
}

} // namespace

// ---- App operators ----

TEST(App, EqualityIsKeyedOnIdAndIsUser)
{
    App a = makeApp(QStringLiteral("id1"), false);
    App b = makeApp(QStringLiteral("id1"), false);
    App c = makeApp(QStringLiteral("id1"), true);   // same Id, different isUser
    App d = makeApp(QStringLiteral("id2"), false);  // different Id, same isUser

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
}

TEST(App, InequalityUsesAndOfBothFields)
{
    // NOTE: Category.h defines operator!= as (app.Id != Id && app.isUser != isUser),
    // i.e. the AND of two inequalities — not the logical negation of operator==.
    // This means two Apps differing in only ONE field are reported as "equal"
    // (operator!= returns false). This is a defect (see delivery note) but the
    // test asserts the ACTUAL behavior so it does not mask the bug.
    App a = makeApp(QStringLiteral("id1"), false);

    App sameIdDiffUser = makeApp(QStringLiteral("id1"), true);   // differ only in isUser
    App diffIdSameUser = makeApp(QStringLiteral("id2"), false); // differ only in Id
    App diffBoth = makeApp(QStringLiteral("id2"), true);         // differ in both

    // Differ in one field → operator!= (actual impl) returns false (claims equal).
    EXPECT_FALSE(a != sameIdDiffUser);
    EXPECT_FALSE(a != diffIdSameUser);
    // Differ in both fields → operator!= returns true.
    EXPECT_TRUE(a != diffBoth);
}

// ---- Category ----

TEST(Category, ConstructorStartsEmpty)
{
    Category cat;
    EXPECT_TRUE(cat.getappItem().isEmpty());
    EXPECT_TRUE(cat.systemAppList().isEmpty());
    EXPECT_TRUE(cat.userAppList().isEmpty());
    EXPECT_TRUE(cat.getName().isEmpty());
}

TEST(Category, SetCategoryEmitsOnlyOnChange)
{
    Category cat;
    QSignalSpy spy(&cat, &Category::categoryNameChanged);

    cat.setCategory(QStringLiteral("browser"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("browser"));
    EXPECT_EQ(cat.getName(), QStringLiteral("browser"));

    // Same value → early return, no signal.
    cat.setCategory(QStringLiteral("browser"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(Category, SetDefaultEmitsOnlyWhenIdChanges)
{
    Category cat;
    QSignalSpy spy(&cat, &Category::defaultChanged);

    const App a = makeApp(QStringLiteral("d1"), false);
    cat.setDefault(a);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).value<App>().Id, QStringLiteral("d1"));  // 消费首信号 + 校验参数

    // Same Id → guarded, no signal, default unchanged.
    const App aSameId = makeApp(QStringLiteral("d1"), true);
    cat.setDefault(aSameId);
    EXPECT_EQ(spy.count(), 0);
    EXPECT_EQ(cat.getDefault().Id, QStringLiteral("d1"));
    EXPECT_FALSE(cat.getDefault().isUser);

    // Different Id → signal, default replaced.
    const App b = makeApp(QStringLiteral("d2"), true);
    cat.setDefault(b);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).value<App>().Id, QStringLiteral("d2"));
    EXPECT_EQ(cat.getDefault().Id, QStringLiteral("d2"));
    EXPECT_TRUE(cat.getDefault().isUser);
}

TEST(Category, AddUserItemRoutesByIsUserAndDeduplicates)
{
    Category cat;
    QSignalSpy spy(&cat, &Category::addedUserItem);

    const App user1 = makeApp(QStringLiteral("u1"), true, QStringLiteral("U1"));
    const App sys1 = makeApp(QStringLiteral("s1"), false, QStringLiteral("S1"));

    cat.addUserItem(user1);
    cat.addUserItem(sys1);

    ASSERT_EQ(spy.count(), 2);
    EXPECT_EQ(cat.userAppList().size(), 1);
    EXPECT_EQ(cat.systemAppList().size(), 1);
    EXPECT_EQ(cat.getappItem().size(), 2);
    EXPECT_EQ(spy.at(0).at(0).value<App>().Id, QStringLiteral("u1"));
    EXPECT_EQ(spy.at(1).at(0).value<App>().Id, QStringLiteral("s1"));

    // Duplicate (same Id + isUser) → guarded, no signal, lists unchanged.
    cat.addUserItem(user1);
    cat.addUserItem(sys1);
    EXPECT_EQ(spy.count(), 2);
    EXPECT_EQ(cat.userAppList().size(), 1);
    EXPECT_EQ(cat.systemAppList().size(), 1);
    EXPECT_EQ(cat.getappItem().size(), 2);
}

TEST(Category, AddUserItemDistinguishesUserAndSystemWithSameId)
{
    // operator== is keyed on Id AND isUser, so an app with the same Id but
    // different isUser is a distinct entry in a different list.
    Category cat;

    const App userX = makeApp(QStringLiteral("X"), true);
    const App sysX = makeApp(QStringLiteral("X"), false);

    cat.addUserItem(userX);
    cat.addUserItem(sysX);

    EXPECT_EQ(cat.userAppList().size(), 1);
    EXPECT_EQ(cat.systemAppList().size(), 1);
    EXPECT_EQ(cat.getappItem().size(), 2);
}

TEST(Category, DelUserItemRemovesAndEmitsOnlyWhenPresent)
{
    Category cat;
    QSignalSpy spy(&cat, &Category::removedUserItem);

    const App user1 = makeApp(QStringLiteral("u1"), true);
    const App sys1 = makeApp(QStringLiteral("s1"), false);
    cat.addUserItem(user1);
    cat.addUserItem(sys1);

    // Remove a user item → signal, removed from user list and applist.
    cat.delUserItem(user1);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(cat.userAppList().size(), 0);
    EXPECT_EQ(cat.getappItem().size(), 1);

    // Remove a system item → signal, removed from system list and applist.
    cat.delUserItem(sys1);
    ASSERT_EQ(spy.count(), 2);
    EXPECT_EQ(cat.systemAppList().size(), 0);
    EXPECT_TRUE(cat.getappItem().isEmpty());

    // Remove something not present → removeOne returns false, no signal.
    cat.delUserItem(user1);
    EXPECT_EQ(spy.count(), 2);
}

TEST(Category, ClearEmitsClearAllOnlyWhenNonEmpty)
{
    Category cat;
    QSignalSpy spy(&cat, &Category::clearAll);

    cat.addUserItem(makeApp(QStringLiteral("u1"), true));
    cat.addUserItem(makeApp(QStringLiteral("s1"), false));

    // Non-empty clear → emits clearAll, all lists emptied.
    cat.clear();
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(cat.userAppList().isEmpty());
    EXPECT_TRUE(cat.systemAppList().isEmpty());
    EXPECT_TRUE(cat.getappItem().isEmpty());

    // Empty clear → guarded (clearFlag false), no signal.
    cat.clear();
    EXPECT_EQ(spy.count(), 1);
}
