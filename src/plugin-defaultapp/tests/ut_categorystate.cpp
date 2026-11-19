// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Category 读取状态与 defaultapp 快照校验的契约测试。
// 覆盖"已确定未设置"与"尚未读取/读取失败"的区分，
// 以及后端真实值（backendDefault）与 UI 回退值（display default）的分离。
#include "category.h"
#include "defaultappdata.h"

#include <QVariantMap>

#include <gtest/gtest.h>

namespace {

const QString kBrowser = QStringLiteral("browser");

App makeApp(const QString &id)
{
    App app;
    app.Id = id;
    app.Name = id;
    app.DisplayName = id;
    return app;
}

} // namespace

TEST(CategoryState, StartsUnread)
{
    Category category;
    EXPECT_EQ(category.readState(), Category::ReadState::Unread);
    EXPECT_FALSE(category.isResolved());
}

TEST(CategoryState, ConfirmedUnsetResolves)
{
    Category category;
    int stateSignals = 0;
    QObject::connect(&category, &Category::readStateChanged, &category,
                     [&stateSignals] { ++stateSignals; });

    // 后端明确返回空值：这是"已确认未设置"，不是"未读取"
    category.setBackendDefault(App());
    EXPECT_EQ(category.readState(), Category::ReadState::Ok);
    EXPECT_TRUE(category.backendDefault().Id.isEmpty());
    EXPECT_EQ(stateSignals, 1);
}

TEST(CategoryState, ReadFailureIsNotResolved)
{
    Category category;
    category.setReadState(Category::ReadState::Failed);
    EXPECT_FALSE(category.isResolved());

    // 失败后再次进入读取中，仍不算已确认
    category.setReadState(Category::ReadState::Unread);
    EXPECT_FALSE(category.isResolved());
}

TEST(CategoryState, BackendDefaultSeparateFromDisplayDefault)
{
    Category category;
    // 后端未设置，UI 回退到第一个系统应用
    category.setBackendDefault(App());
    category.setDisplayDefault(makeApp(QStringLiteral("fallback.desktop")));

    EXPECT_TRUE(category.backendDefault().Id.isEmpty()) << "数据通道必须看到后端真实值";
    EXPECT_EQ(category.getDefault().Id, QStringLiteral("fallback.desktop"));
}

TEST(CategoryState, ClearingValueEmitsBackendChange)
{
    Category category;
    category.setBackendDefault(makeApp(QStringLiteral("a.desktop")));

    int backendSignals = 0;
    QString lastValue = QStringLiteral("sentinel");
    QObject::connect(&category, &Category::backendDefaultChanged, &category,
                     [&backendSignals, &lastValue](const App &app) {
                         ++backendSignals;
                         lastValue = app.Id;
                     });

    // 有值 -> 未设置：必须发出空值变更，而不是静默保留旧值
    category.setBackendDefault(App());
    EXPECT_EQ(backendSignals, 1);
    EXPECT_TRUE(lastValue.isEmpty());
    EXPECT_TRUE(category.backendDefault().Id.isEmpty());
}

TEST(CategoryState, SetCategoryDoesNotResolve)
{
    // 只有读取路径能改变读取状态；setCategory 只改显示名
    Category category;
    category.setCategory(QStringLiteral("Terminal"));
    EXPECT_FALSE(category.isResolved());
}

TEST(CategoryState, DefaultWriteResolves)
{
    Category category;
    category.setDefault(makeApp(QStringLiteral("a.desktop")));
    EXPECT_TRUE(category.isResolved());
    EXPECT_EQ(category.backendDefault().Id, QStringLiteral("a.desktop"));
}

// ---- DefaultAppData ----

TEST(DefaultAppLookup, RejectsUnknownKeyAndMissingApp)
{
    DefaultAppData data;
    App out;

    EXPECT_FALSE(data.lookupApp(QStringLiteral("nope"), QStringLiteral("a.desktop"), &out));
    EXPECT_FALSE(data.lookupApp(kBrowser, QStringLiteral("absent.desktop"), &out));
    EXPECT_FALSE(data.lookupApp(kBrowser, QString(), &out));
}

TEST(DefaultAppLookup, UnresolvedKeyHasNoSnapshot)
{
    // 未 attach 模型时快照为空：任何目标都不应通过校验，
    // 而不是回退去读主线程的 Category。
    DefaultAppData data;
    App out;
    EXPECT_FALSE(data.lookupApp(kBrowser, QStringLiteral("a.desktop"), &out));
    EXPECT_FALSE(data.isReady());
}

TEST(DefaultAppData, BackendNameMappingIsSingleSource)
{
    // 键到后端类别名只有一份表；未知键返回空串
    EXPECT_EQ(DefaultAppData::backendNameForKey(kBrowser), QStringLiteral("Browser"));
    EXPECT_EQ(DefaultAppData::backendNameForKey(QStringLiteral("terminal")),
              QStringLiteral("Terminal"));
    EXPECT_TRUE(DefaultAppData::backendNameForKey(QStringLiteral("nope")).isEmpty());
    EXPECT_TRUE(DefaultAppData::supportedKeys().contains(kBrowser));
}
