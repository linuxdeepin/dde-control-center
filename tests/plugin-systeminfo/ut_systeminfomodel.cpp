#include "interface/namespace.h"
#include "src/plugin-systeminfo/operation/systeminfomodel.h"

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DCC_USE_NAMESPACE
class Test_SystemInfoModel: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    SystemInfoModel *m_model = nullptr;
};

void Test_SystemInfoModel::SetUp()
{
    m_model = new SystemInfoModel();
}

void Test_SystemInfoModel::TearDown()
{
    delete m_model;
    m_model = nullptr;
}


TEST_F(Test_SystemInfoModel, setHostName)
{
    QSignalSpy spy1(m_model, SIGNAL(hostNameChanged(const QString&)));
    m_model->setHostName("swq-pc");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->hostName();
    EXPECT_EQ(value, "swq-pc");
}

TEST_F(Test_SystemInfoModel, setKernel)
{
    QSignalSpy spy1(m_model, SIGNAL(kernelChanged(const QString&)));
    m_model->setKernel("4.19.0-amd64-desktop");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->kernel();
    EXPECT_EQ(value, "4.19.0-amd64-desktop");
}

TEST_F(Test_SystemInfoModel, setLicenseState)
{
    QSignalSpy spy1(m_model, SIGNAL(licenseStateChanged(ActiveState)));
    m_model->setLicenseState(ActiveState::Authorized);
    EXPECT_EQ(spy1.count(), 1);
    ActiveState value = m_model->licenseState();
    int b = value;
    int a = ActiveState::Authorized;
    EXPECT_EQ(a, b);
}

TEST_F(Test_SystemInfoModel, setMemory)
{
    QSignalSpy spy1(m_model, SIGNAL(memoryChanged(const QString&)));
    m_model->setMemory(10000,8000);
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->memory();
    EXPECT_NE(value, "");
}

TEST_F(Test_SystemInfoModel, setProcessor)
{
    QSignalSpy spy1(m_model, SIGNAL(processorChanged(const QString&)));
    m_model->setProcessor("Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz333");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->processor();
    EXPECT_EQ(value, "Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz333");
}

TEST_F(Test_SystemInfoModel, setProductName)
{
    QSignalSpy spy1(m_model, SIGNAL(productNameChanged(const QString&)));
    m_model->setProductName("统信桌面操作系统");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->productName();
    EXPECT_EQ(value, "统信桌面操作系统");
}

TEST_F(Test_SystemInfoModel, setType)
{
    QSignalSpy spy1(m_model, SIGNAL(typeChanged(const QString&)));
    m_model->setType(64);
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->type();
    EXPECT_EQ(value, QString("%1").arg(64));
}

TEST_F(Test_SystemInfoModel, setVersion)
{
    QSignalSpy spy1(m_model, SIGNAL(versionChanged(const QString&)));
    m_model->setVersion("专业版 (1040)");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->version();
    EXPECT_EQ(value, "专业版 (1040)");
}

TEST_F(Test_SystemInfoModel, setVersionNumber)
{
    QSignalSpy spy1(m_model, SIGNAL(versionNumberChanged(const QString&)));
    m_model->setVersionNumber("2011");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->versionNumber();
    EXPECT_EQ(value, "2011");
}
