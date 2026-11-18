// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 数据流请求测试：验证“一次请求恰好一次回复”以及业务状态到 DBus 错误的映射。
// 使用私有会话总线，不依赖控制中心可执行程序。
#include "dccdatarequest.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusContext>
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

#include <gtest/gtest.h>

#include <memory>

using namespace dccV25;

namespace {

const QString kService = QStringLiteral("org.deepin.dcc.DataRequestTest");
const QString kPath = QStringLiteral("/Test");
const QString kInterface = QStringLiteral("org.deepin.dcc.DataRequestTest");

// 把 request 的 finish* 调用接到真实总线上
class ReplyService : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dcc.DataRequestTest")
public:
    int replies = 0;

public Q_SLOTS:
    void Ok()
    {
        auto *request = new DataRequest(1, QStringLiteral("defaultapp"), DataOperation::Get,
                                        QVariantMap(), message().service(), QStringLiteral("/bin/ut-datarequest"),
                                        connection(), message(), this);
        setDelayedReply(true);
        request->finish({ QVariant(QVariantMap{ { QStringLiteral("browser"), QStringLiteral("a.desktop") } }) });
        ++replies;
        // 第二次调用必须是空操作
        request->finish({ QVariant(QVariantMap{ { QStringLiteral("browser"), QStringLiteral("ignored") } }) });
    }
    void Unsupported()
    {
        auto *request = new DataRequest(2, QStringLiteral("legacy"), DataOperation::Get,
                                        QVariantMap(), message().service(), QStringLiteral("/bin/ut-datarequest"),
                                        connection(), message(), this);
        setDelayedReply(true);
        request->finishFromReply({ DataStatus::NotSupported, {}, {} });
        ++replies;
    }
    void NotReady()
    {
        auto *request = new DataRequest(3, QStringLiteral("defaultapp"), DataOperation::Get,
                                        QVariantMap(), message().service(), QStringLiteral("/bin/ut-datarequest"),
                                        connection(), message(), this);
        setDelayedReply(true);
        request->finishFromReply({ DataStatus::NotReady, {}, QStringLiteral("loading") });
        ++replies;
    }
};

class DataRequestTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QDBusConnection bus = QDBusConnection::sessionBus();
        ASSERT_TRUE(bus.isConnected()) << "需要会话总线；测试应通过 dbus-run-session 启动";
        ASSERT_TRUE(bus.registerService(kService));
        m_service = std::make_unique<ReplyService>();
        ASSERT_TRUE(bus.registerObject(kPath, m_service.get(), QDBusConnection::ExportAllSlots));
        m_client = QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                                 QStringLiteral("dcc-data-request-test"));
        ASSERT_TRUE(m_client.isConnected());
    }

    void TearDown() override
    {
        QDBusConnection bus = QDBusConnection::sessionBus();
        bus.unregisterObject(kPath);
        bus.unregisterService(kService);
    }

    // 服务与客户端在同一进程：必须用异步调用并转事件循环，阻塞调用会自己把自己锁死
    QDBusMessage call(const QString &method)
    {
        QDBusMessage result;
        QEventLoop loop;
        QDBusPendingCallWatcher watcher(
                m_client.asyncCall(QDBusMessage::createMethodCall(kService, kPath, kInterface, method)));
        QObject::connect(&watcher, &QDBusPendingCallWatcher::finished, &loop, [&] {
            result = watcher.reply();
            loop.quit();
        });
        QTimer::singleShot(2000, &loop, &QEventLoop::quit);
        loop.exec();
        return result;
    }

    std::unique_ptr<ReplyService> m_service;
    QDBusConnection m_client { QDBusConnection::sessionBus() };
};

} // namespace

TEST_F(DataRequestTest, SuccessReplyIsDeliveredOnce)
{
    const auto reply = call(QStringLiteral("Ok"));
    ASSERT_EQ(reply.type(), QDBusMessage::ReplyMessage) << reply.errorMessage().toStdString();
    ASSERT_EQ(reply.arguments().size(), 1);
    const auto map = qdbus_cast<QVariantMap>(reply.arguments().first());
    EXPECT_EQ(map.value(QStringLiteral("browser")).toString(), QStringLiteral("a.desktop"));
}

TEST_F(DataRequestTest, NotSupportedMapsToDBusError)
{
    const auto reply = call(QStringLiteral("Unsupported"));
    ASSERT_EQ(reply.type(), QDBusMessage::ErrorMessage);
    EXPECT_EQ(reply.errorName(), QStringLiteral("org.freedesktop.DBus.Error.NotSupported"));
    EXPECT_FALSE(reply.errorMessage().isEmpty());
}

TEST_F(DataRequestTest, NotReadyMapsToModuleError)
{
    const auto reply = call(QStringLiteral("NotReady"));
    ASSERT_EQ(reply.type(), QDBusMessage::ErrorMessage);
    EXPECT_EQ(reply.errorName(), QStringLiteral("org.deepin.dde.ControlCenter1.Error.NotReady"));
    EXPECT_EQ(reply.errorMessage(), QStringLiteral("loading"));
}

TEST(DataRequestLimits, DeclaredBoundsMatchPlan)
{
    // 计划 §6.3 冻结的初始边界；改动必须同时更新文档
    EXPECT_EQ(DataRequest::kTimeoutMs, 20000);
    EXPECT_EQ(DataRequest::kMaxPendingTotal, 64);
    EXPECT_EQ(DataRequest::kMaxPendingPerModule, 8);
    EXPECT_EQ(DataRequest::kMaxParamKeys, 16);
}

TEST_F(DataRequestTest, WhitelistPolicyFailsClosedForSet)
{
    WhitelistAccessPolicy policy;
    // 名单为空时 Get 放行、Set 拒绝
    EXPECT_TRUE(policy.isAllowed(QStringLiteral("defaultapp"), DataOperation::Get, QStringLiteral(":1.1")));
    EXPECT_FALSE(policy.isAllowed(QStringLiteral("defaultapp"), DataOperation::Set, QStringLiteral(":1.1")));

    policy.allowModule(QStringLiteral("defaultapp"));
    EXPECT_TRUE(policy.isAllowed(QStringLiteral("defaultapp"), DataOperation::Set, QStringLiteral(":1.1")));
    EXPECT_FALSE(policy.isAllowed(QStringLiteral("other"), DataOperation::Set, QStringLiteral(":1.1")));
}

// gtest_main 不带 QCoreApplication；DBus 与事件循环都需要它
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "ut_datarequest.moc"
