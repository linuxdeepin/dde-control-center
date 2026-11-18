// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// defaultapp 数据通道契约测试：请求为用户键值映射，Get 只取键集合，
// Set 逐项返回结果。不依赖真实 MIME 后端或桌面服务。
#include "category.h"
#include "defaultappdata.h"
#include "defappmodel.h"

#include <QHash>
#include <QVariantMap>

#include <gtest/gtest.h>

namespace {

const QString kBrowser = QStringLiteral("browser");
const QString kTerminal = QStringLiteral("terminal");
constexpr int kBrowserIndex = 0;
constexpr int kTerminalIndex = 6;

QVariantMap request(std::initializer_list<QString> keys)
{
    QVariantMap map;
    for (const auto &key : keys) {
        map.insert(key, QString());
    }
    return map;
}

App makeApp(const QString &id)
{
    App app;
    app.Id = id;
    app.Name = id;
    app.DisplayName = id;
    return app;
}

// 七个类别都成功读取，默认项为空（已确认未设置）
QHash<int, Category *> readyCategories(QList<Category *> &storage)
{
    QHash<int, Category *> map;
    for (int i = 0; i < 7; ++i) {
        auto *category = new Category();
        category->setBackendDefault(App());
        storage.append(category);
        map.insert(i, category);
    }
    return map;
}

} // namespace

TEST(DefaultAppData, ReadBeforeReadyReturnsNotReady)
{
    DefaultAppData data;
    EXPECT_EQ(data.read(request({ kBrowser })).status, dccV25::DataStatus::NotReady);
}

TEST(DefaultAppData, RejectsInvalidRequests)
{
    DefaultAppData data;
    // 空请求
    EXPECT_EQ(data.read({}).status, dccV25::DataStatus::InvalidArgs);
    // 未知键
    EXPECT_EQ(data.read(request({ QStringLiteral("nope") })).status,
              dccV25::DataStatus::InvalidArgs);
    // 已知键 + 未知键：整体拒绝
    EXPECT_EQ(data.read(request({ kBrowser, QStringLiteral("nope") })).status,
              dccV25::DataStatus::InvalidArgs);
}

TEST(DefaultAppData, ReadProjectsRequestedKeys)
{
    DefaultAppData data;
    QList<Category *> owned;
    data.setCategories(readyCategories(owned));
    ASSERT_TRUE(data.isReady());

    const auto reply = data.read(request({ kBrowser, kTerminal }));
    ASSERT_EQ(reply.status, dccV25::DataStatus::Ok) << reply.message.toStdString();
    EXPECT_EQ(reply.value.size(), 2);
    EXPECT_TRUE(reply.value.contains(kBrowser));
    EXPECT_TRUE(reply.value.contains(kTerminal));

    qDeleteAll(owned);
}

TEST(DefaultAppData, ReadIgnoresRequestValues)
{
    DefaultAppData data;
    QList<Category *> owned;
    data.setCategories(readyCategories(owned));

    // Get 只看键集合；值被忽略，非空值也不报错
    QVariantMap withValues;
    withValues.insert(kBrowser, QStringLiteral("ignored"));
    const auto reply = data.read(withValues);
    EXPECT_EQ(reply.status, dccV25::DataStatus::Ok);
    EXPECT_TRUE(reply.value.contains(kBrowser));

    qDeleteAll(owned);
}

TEST(DefaultAppData, FailedKeyIsNotReadyButOtherKeysStillAnswer)
{
    DefaultAppData data;
    QList<Category *> owned;
    auto categories = readyCategories(owned);
    // 只让 terminal 读取失败：按请求键分别判定，不因单个键失败而封锁其他键
    categories.value(kTerminalIndex)->setReadState(Category::ReadState::Failed);
    data.setCategories(categories);
    EXPECT_FALSE(data.isReady()) << "任一类未读取成功时模块整体不就绪";

    EXPECT_EQ(data.read(request({ kBrowser })).status, dccV25::DataStatus::Ok);
    EXPECT_EQ(data.read(request({ kTerminal })).status, dccV25::DataStatus::NotReady);

    qDeleteAll(owned);
}

TEST(DefaultAppData, FailedReadIsNotReportedAsUnset)
{
    DefaultAppData data;
    QList<Category *> owned;
    auto categories = readyCategories(owned);
    Category *terminal = categories.value(kTerminalIndex);
    terminal->setBackendDefault(makeApp(QStringLiteral("old.desktop")));
    // 由"有值"转为"读取失败"：不能返回旧的默认值，也不能谎报空值
    terminal->setReadState(Category::ReadState::Failed);
    data.setCategories(categories);

    const auto reply = data.read(request({ kTerminal }));
    EXPECT_EQ(reply.status, dccV25::DataStatus::NotReady);

    qDeleteAll(owned);
}

TEST(DefaultAppData, ConfirmedUnsetReturnsEmptyString)
{
    DefaultAppData data;
    QList<Category *> owned;
    data.setCategories(readyCategories(owned));

    const auto reply = data.read(request({ kTerminal }));
    ASSERT_EQ(reply.status, dccV25::DataStatus::Ok);
    EXPECT_TRUE(reply.value.value(kTerminal).toString().isEmpty())
            << "已确认未设置必须返回空串，而不是 NotReady";

    qDeleteAll(owned);
}

TEST(DefaultAppData, BackendValueNotUiFallback)
{
    DefaultAppData data;
    QList<Category *> owned;
    auto categories = readyCategories(owned);
    Category *browser = categories.value(kBrowserIndex);
    // 后端未设置，UI 回退展示了第一个系统应用
    browser->setBackendDefault(App());
    browser->setDisplayDefault(makeApp(QStringLiteral("fallback.desktop")));
    data.setCategories(categories);

    const auto reply = data.read(request({ kBrowser }));
    ASSERT_EQ(reply.status, dccV25::DataStatus::Ok);
    EXPECT_TRUE(reply.value.value(kBrowser).toString().isEmpty())
            << "Get 不能把 UI 回退项当成后端默认值";

    qDeleteAll(owned);
}

TEST(DefaultAppData, ValueChangeAndClearAreNotified)
{
    DefaultAppData data;
    QList<Category *> owned;
    auto categories = readyCategories(owned);
    Category *browser = categories.value(kBrowserIndex);
    data.setCategories(categories);

    QVariantMap lastNotification;
    int fired = 0;
    QObject::connect(&data, &DefaultAppData::propertiesChanged, &data,
                     [&fired, &lastNotification](const QVariantMap &properties) {
                         ++fired;
                         lastNotification = properties;
                     });

    browser->setBackendDefault(makeApp(QStringLiteral("a.desktop")));
    ASSERT_EQ(fired, 1);
    EXPECT_EQ(lastNotification.value(kBrowser).toString(), QStringLiteral("a.desktop"));

    // 有值 -> 未设置：必须发出清空通知，而不是静默保留旧值
    browser->setBackendDefault(App());
    EXPECT_EQ(fired, 2);
    EXPECT_TRUE(lastNotification.contains(kBrowser));
    EXPECT_TRUE(lastNotification.value(kBrowser).toString().isEmpty());

    qDeleteAll(owned);
}

TEST(DefaultAppData, RepeatedSameValueDoesNotNotify)
{
    DefaultAppData data;
    QList<Category *> owned;
    auto categories = readyCategories(owned);
    Category *browser = categories.value(kBrowserIndex);
    data.setCategories(categories);

    int fired = 0;
    QObject::connect(&data, &DefaultAppData::propertiesChanged, &data,
                     [&fired] { ++fired; });

    browser->setBackendDefault(makeApp(QStringLiteral("a.desktop")));
    ASSERT_EQ(fired, 1);
    browser->setBackendDefault(makeApp(QStringLiteral("a.desktop")));
    EXPECT_EQ(fired, 1) << "值未变化时不应重复通知";

    qDeleteAll(owned);
}

TEST(DefaultAppData, ValidateSetAcceptsMultipleKeys)
{
    QVariantMap target;
    target.insert(kBrowser, QStringLiteral("a.desktop"));
    target.insert(kTerminal, QStringLiteral("b.desktop"));
    EXPECT_EQ(DefaultAppData::validateSet(target).status, dccV25::DataStatus::Ok);
}

TEST(DefaultAppData, ValidateSetRejectsInvalidInput)
{
    EXPECT_EQ(DefaultAppData::validateSet({}).status, dccV25::DataStatus::InvalidArgs);

    QVariantMap unknown;
    unknown.insert(QStringLiteral("nope"), QStringLiteral("a.desktop"));
    EXPECT_EQ(DefaultAppData::validateSet(unknown).status, dccV25::DataStatus::InvalidArgs);

    // 空值不能作为写入目标
    QVariantMap emptyValue;
    emptyValue.insert(kBrowser, QString());
    EXPECT_EQ(DefaultAppData::validateSet(emptyValue).status, dccV25::DataStatus::InvalidArgs);

    // 一项非法则整体拒绝，避免部分写入
    QVariantMap mixed;
    mixed.insert(kBrowser, QStringLiteral("a.desktop"));
    mixed.insert(kTerminal, QString());
    EXPECT_EQ(DefaultAppData::validateSet(mixed).status, dccV25::DataStatus::InvalidArgs);
}

TEST(DefaultAppData, DeactivateRestoresNotReady)
{
    DefaultAppData data;
    QList<Category *> owned;
    data.setCategories(readyCategories(owned));
    ASSERT_TRUE(data.isReady());

    data.markUnavailable(QStringLiteral("deactivated"));
    EXPECT_FALSE(data.isReady());
    EXPECT_EQ(data.read(request({ kBrowser })).status, dccV25::DataStatus::NotReady);

    qDeleteAll(owned);
}

TEST(DefaultAppData, SupportsOnlyKnownKeys)
{
    EXPECT_EQ(DefaultAppData::backendNameForKey(kBrowser), QStringLiteral("Browser"));
    EXPECT_EQ(DefaultAppData::backendNameForKey(kTerminal), QStringLiteral("Terminal"));
    EXPECT_TRUE(DefaultAppData::backendNameForKey(QStringLiteral("nope")).isEmpty());
    EXPECT_EQ(DefaultAppData::supportedKeys().size(), 7);
}

// ---- DefAppModel 读取失败重试退避 ----
// 纯策略测试：只验证间隔序列，不需要构造依赖 DBus 后端的模型。
TEST(RetryBackoff, DoublesAndCaps)
{
    int delay = DefAppModel::kInitialRetryDelayMs;
    EXPECT_EQ(delay, 2000);
    EXPECT_EQ(DefAppModel::nextRetryDelay(delay), 4000);
    EXPECT_EQ(DefAppModel::nextRetryDelay(4000), 8000);
    EXPECT_EQ(DefAppModel::nextRetryDelay(8000), 16000);
    EXPECT_EQ(DefAppModel::nextRetryDelay(16000), DefAppModel::kMaxRetryDelayMs);
}

TEST(RetryBackoff, NeverExceedsCapHoweverManyFailures)
{
    // 没有硬次数上限：持续失败也必须永远保持一个可恢复的探测间隔，
    // 否则一次长时间故障会让模块永久 NotReady。
    int delay = DefAppModel::kInitialRetryDelayMs;
    for (int i = 0; i < 100; ++i) {
        delay = DefAppModel::nextRetryDelay(delay);
        ASSERT_LE(delay, DefAppModel::kMaxRetryDelayMs) << "iteration " << i;
        ASSERT_GE(delay, DefAppModel::kInitialRetryDelayMs) << "iteration " << i;
    }
    EXPECT_EQ(delay, DefAppModel::kMaxRetryDelayMs);
}
