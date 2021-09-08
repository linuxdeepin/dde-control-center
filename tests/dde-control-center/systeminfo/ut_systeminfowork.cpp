#define private public
#include "../src/frame/modules/systeminfo/systeminfomodel.h"
#include "../src/frame/modules/systeminfo/systeminfowork.h"
#undef private

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace dcc::systeminfo;

class Test_SystemInfoWork: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    SystemInfoWork *m_work = nullptr;
    SystemInfoModel *m_model = nullptr;
};

void Test_SystemInfoWork::SetUp()
{
    m_model = new dcc::systeminfo::SystemInfoModel;
    m_work = new SystemInfoWork(m_model);
}

void Test_SystemInfoWork::TearDown()
{
    delete m_work;
    m_work = nullptr;

    delete m_model;
	m_model = nullptr;
}

TEST_F(Test_SystemInfoWork, setBootDelay)
{
    QSignalSpy spy1(m_work, SIGNAL(requestSetAutoHideDCC(bool)));
    m_work->setBootDelay(true);
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_SystemInfoWork, setEnableTheme)
{
    QSignalSpy spy1(m_work, SIGNAL(requestSetAutoHideDCC(bool)));
    m_work->setEnableTheme(true);
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_SystemInfoWork, setDefaultEntry)
{
    QSignalSpy spy1(m_work, SIGNAL(requestSetAutoHideDCC(bool)));
    m_work->setDefaultEntry("test");
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_SystemInfoWork, setBackground)
{
    QSignalSpy spy1(m_work, SIGNAL(requestSetAutoHideDCC(bool)));
    m_work->setBackground("test");
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_SystemInfoWork, modeltest)
{
    QSignalSpy spy1(m_model, SIGNAL(setHostNameChanged(const QString&)));
    m_model->setHostNameChanged("test");
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_SystemInfoWork, full)
{
//    EXPECT_NO_THROW(m_work->showActivatorDialog());
    EXPECT_NO_THROW(m_work->licenseStateChangeSlot());
    EXPECT_NO_THROW(m_work->getEntryTitles());
    EXPECT_NO_THROW(m_work->onBackgroundChanged());
    EXPECT_NO_THROW(m_work->grubServerFinished());
    EXPECT_NO_THROW(m_work->getLicenseState());

    EXPECT_NO_THROW(m_work->activate());
}