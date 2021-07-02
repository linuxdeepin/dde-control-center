#include "proxycontroller.h"
#include "networkcontroller.h"

#include <gtest/gtest.h>

using namespace dde::network;

class Tst_ProxyController : public testing::Test
{
public:
    void SetUp() override
    {
        m_controller = NetworkController::instance()->proxyController();
    }

    void TearDown() override
    {
        m_controller = nullptr;
    }

public:
    ProxyController *m_controller;
};

TEST_F(Tst_ProxyController, dslcontroll_test)
{
    m_controller->querySysProxyData();

    SysProxyConfig config = m_controller->proxy(SysProxyType::Http);
    QString autoProxy = m_controller->autoProxy();
    AppProxyConfig appConf = m_controller->appProxy();
    ProxyMethod method = m_controller->proxyMethod();
    bool appProxyExist = m_controller->appProxyExist();

    m_controller->setProxy(SysProxyType::Ftp, "127.0.0.1", "2512");

    AppProxyConfig appConfig;
    appConfig.ip = "127.0.0.1";
    appConfig.port = 8012;
    appConfig.type = AppProxyType::Http;
    m_controller->setAppProxy(appConfig);

    m_controller->setProxyMethod(ProxyMethod::Auto);

    m_controller->setProxyIgnoreHosts("192.168.0.11");

    m_controller->setAutoProxy("https://it.uniontech.com/proxy/proxy.pac");
}
