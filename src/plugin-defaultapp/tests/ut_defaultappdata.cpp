// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// defaultapp 数据通道的取值与校验来源：Category 的默认项与候选列表。
#include "category.h"

#include <QVariantMap>

#include <gtest/gtest.h>

namespace {

App makeApp(const QString &id)
{
    App app;
    app.Id = id;
    app.Name = id;
    app.DisplayName = id;
    return app;
}

} // namespace

// 写默认项：只在 Id 变化时通知，避免重复写入产生重复快照刷新
TEST(CategoryDefault, SetDefaultNotifiesOnlyOnIdChange)
{
    Category category;
    int fired = 0;
    QObject::connect(&category, &Category::defaultChanged, &category,
                     [&fired] { ++fired; });

    category.setDefault(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(fired, 1);
    EXPECT_EQ(category.getDefault().Id, QStringLiteral("a.desktop"));

    // 同一个 Id 重复设置：不重复通知
    category.setDefault(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(fired, 1);

    // 换成另一个 Id：通知
    category.setDefault(makeApp(QStringLiteral("b.desktop")));
    EXPECT_EQ(fired, 2);
}

// 未设置时 getDefault() 返回空 Id，这是 Get 报空串的依据
TEST(CategoryDefault, UnsetDefaultIsEmptyId)
{
    Category category;
    EXPECT_TRUE(category.getDefault().Id.isEmpty());
}

// 类别名是写入路径的类别标识；改名要通知，重复设置不通知
TEST(CategoryName, SetCategoryNotifiesOnce)
{
    Category category;
    int fired = 0;
    QObject::connect(&category, &Category::categoryNameChanged, &category,
                     [&fired] { ++fired; });

    category.setCategory(QStringLiteral("Browser"));
    EXPECT_EQ(fired, 1);
    EXPECT_EQ(category.getName(), QStringLiteral("Browser"));

    category.setCategory(QStringLiteral("Browser"));
    EXPECT_EQ(fired, 1);
}

// 候选列表：Set 只接受候选里的应用，候选增删要同步通知
TEST(CategoryApps, AddAndRemoveUserItems)
{
    Category category;
    int added = 0;
    int removed = 0;
    QObject::connect(&category, &Category::addedUserItem, &category,
                     [&added] { ++added; });
    QObject::connect(&category, &Category::removedUserItem, &category,
                     [&removed] { ++removed; });

    category.addUserItem(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(added, 1);
    EXPECT_EQ(category.getappItem().size(), 1);

    // 重复添加同一个 App 不增加候选，也不再通知
    category.addUserItem(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(added, 1);
    EXPECT_EQ(category.getappItem().size(), 1);

    category.delUserItem(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(removed, 1);
    EXPECT_TRUE(category.getappItem().isEmpty());
}

// 删除不存在的候选：不应发出移除通知
TEST(CategoryApps, RemoveAbsentItemDoesNotNotify)
{
    Category category;
    int removed = 0;
    QObject::connect(&category, &Category::removedUserItem, &category,
                     [&removed] { ++removed; });

    category.delUserItem(makeApp(QStringLiteral("absent.desktop")));
    EXPECT_EQ(removed, 0);
}

// 系统应用与用户应用分列：isUser 决定进入哪个列表
TEST(CategoryApps, SystemAndUserListsAreSeparate)
{
    Category category;
    App systemApp = makeApp(QStringLiteral("sys.desktop"));
    App userApp = makeApp(QStringLiteral("usr.desktop"));
    userApp.isUser = true;

    category.addUserItem(systemApp);
    category.addUserItem(userApp);

    EXPECT_EQ(category.systemAppList().size(), 1);
    EXPECT_EQ(category.userAppList().size(), 1);
    EXPECT_EQ(category.getappItem().size(), 2) << "候选全集应包含两者";
    EXPECT_EQ(category.systemAppList().first().Id, QStringLiteral("sys.desktop"));
    EXPECT_EQ(category.userAppList().first().Id, QStringLiteral("usr.desktop"));
}

// 同名不同来源（isUser）视为不同候选：Id 相同也要分别保留
TEST(CategoryApps, SameIdDifferentSourceAreDistinct)
{
    Category category;
    App systemApp = makeApp(QStringLiteral("a.desktop"));
    App userApp = makeApp(QStringLiteral("a.desktop"));
    userApp.isUser = true;

    category.addUserItem(systemApp);
    category.addUserItem(userApp);

    EXPECT_EQ(category.getappItem().size(), 2);
    EXPECT_EQ(category.systemAppList().size(), 1);
    EXPECT_EQ(category.userAppList().size(), 1);

    // 移除其中一项不影响另一项
    category.delUserItem(userApp);
    EXPECT_EQ(category.getappItem().size(), 1);
    EXPECT_EQ(category.systemAppList().size(), 1);
    EXPECT_TRUE(category.userAppList().isEmpty());
}

// 清空候选：非空时发 clearAll，空时不重复发
TEST(CategoryApps, ClearNotifiesOnlyWhenNonEmpty)
{
    Category category;
    int cleared = 0;
    QObject::connect(&category, &Category::clearAll, &category,
                     [&cleared] { ++cleared; });

    category.clear();
    EXPECT_EQ(cleared, 0) << "已经是空的，不应发通知";

    category.addUserItem(makeApp(QStringLiteral("a.desktop")));
    category.clear();
    EXPECT_EQ(cleared, 1);
    EXPECT_TRUE(category.getappItem().isEmpty());
}
