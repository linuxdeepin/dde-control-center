#define private public
#include "../src/frame/modules/systeminfo/systeminfomodel.h"
#undef private

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace dcc::systeminfo;

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

TEST_F(Test_SystemInfoModel, setBootDelay)
{
    QSignalSpy spy1(m_model, SIGNAL(bootDelayChanged(const int )));
    m_model->setBootDelay(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->bootDelay();
    EXPECT_EQ(value, true);
}

TEST_F(Test_SystemInfoModel, setDefaultEntry)
{
    QSignalSpy spy1(m_model, SIGNAL(defaultEntryChanged(const QString&)));
    m_model->setDefaultEntry("test");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->defaultEntry();
    EXPECT_EQ(value, "test");
}

TEST_F(Test_SystemInfoModel, setDisk)
{
    QSignalSpy spy1(m_model, SIGNAL(diskChanged(const QString&)));
    m_model->setDisk(83.5);
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->disk();
    EXPECT_NE(value, "");
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
    m_model->setLicenseState(dcc::systeminfo::ActiveState::Authorized);
    EXPECT_EQ(spy1.count(), 1);
    ActiveState value = m_model->licenseState();
    int b = value;
    int a  =dcc::systeminfo::ActiveState::Authorized;
    EXPECT_EQ(a, b);
}

TEST_F(Test_SystemInfoModel, setMemory)
{
    QSignalSpy spy1(m_model, SIGNAL(memoryChanged(const QString&)));
    m_model->setMemory(16,15.6);
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

TEST_F(Test_SystemInfoModel, setThemeEnabled)
{
    QSignalSpy spy1(m_model, SIGNAL(themeEnabledChanged(const bool)));
    m_model->setThemeEnabled(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->themeEnabled();
    EXPECT_EQ(value, true);
}

TEST_F(Test_SystemInfoModel, setType)
{
    QSignalSpy spy1(m_model, SIGNAL(typeChanged(const QString&)));
    m_model->setType(64);
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->type();
    EXPECT_EQ(value, QString("%1").arg(64));
}

TEST_F(Test_SystemInfoModel, setUpdating)
{
    QSignalSpy spy1(m_model, SIGNAL(updatingChanged(const bool)));
    m_model->setUpdating(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->updating();
    EXPECT_EQ(value, true);
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

TEST_F(Test_SystemInfoModel, setEntryLists)
{
    QSignalSpy spy1(m_model, SIGNAL(entryListsChanged(const QStringList&)));
    QStringList list;
    list.append("test1");
    list.append("test2");
    m_model->setEntryLists(list);
    EXPECT_EQ(spy1.count(), 1);
    QStringList value = m_model->entryLists();
    EXPECT_EQ(list.at(0), "test1");
    EXPECT_EQ(list.at(1), "test2");
}

TEST_F(Test_SystemInfoModel, setDistroID)
{
    QSignalSpy spy1(m_model, SIGNAL(distroidChanged(const QString&)));
    m_model->setDistroID("test");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->distroid();
    EXPECT_EQ(value, "test");
}

TEST_F(Test_SystemInfoModel, setDistroVer)
{
    QSignalSpy spy1(m_model, SIGNAL(distroverChanged(const QString&)));
    m_model->setDistroVer("test");
    EXPECT_EQ(spy1.count(), 1);
    QString value = m_model->distrover();
    EXPECT_EQ(value, "test");
}

TEST_F(Test_SystemInfoModel, setBackground)
{
    QSignalSpy spy1(m_model, SIGNAL(backgroundChanged(const QPixmap&)));
    QPixmap icon = QIcon::fromTheme("dialog-error").pixmap(QSize(48, 48));
    m_model->setBackground(icon);
    EXPECT_EQ(spy1.count(), 1);
    QPixmap value = m_model->background();
    EXPECT_EQ(value.height(), icon.height());
}