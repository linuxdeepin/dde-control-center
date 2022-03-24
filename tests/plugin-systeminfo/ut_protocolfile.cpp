#include "src/plugin-systeminfo/window/protocolfile.h"

#include <QTest>
#include <gtest/gtest.h>


class Test_ProtocolFile: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    ProtocolFile *m_pro = nullptr;
};

void Test_ProtocolFile::SetUp()
{
    m_pro = new ProtocolFile();
}

void Test_ProtocolFile::TearDown()
{
    delete m_pro;
    m_pro = nullptr;
}

TEST_F(Test_ProtocolFile, getPrivacyFile)
{
    QString zhCN_Content = "";
    QString enUS_Content = "";
    EXPECT_NO_THROW(m_pro->getPrivacyFile(zhCN_Content,enUS_Content));
}

TEST_F(Test_ProtocolFile, getUserExpContent)
{
    EXPECT_NO_THROW(m_pro->getUserExpContent());
}

TEST_F(Test_ProtocolFile, getEnduserAgreement)
{
    EXPECT_NO_THROW(m_pro->getEnduserAgreement());
}

TEST_F(Test_ProtocolFile, getEulerEnduserAgreement)
{
    EXPECT_NO_THROW(m_pro->getEulerEnduserAgreement());
}
