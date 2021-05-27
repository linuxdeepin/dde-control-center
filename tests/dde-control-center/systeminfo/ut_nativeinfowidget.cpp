#include "../src/frame/window/modules/systeminfo/nativeinfowidget.h"
#include "../src/frame/modules/systeminfo/systeminfomodel.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace dccV20::systeminfo;
using namespace dcc::systeminfo;

DWIDGET_USE_NAMESPACE

class Test_NativeInfoWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    dccV20::systeminfo::NativeInfoWidget *m_widget = nullptr;
    dcc::systeminfo::SystemInfoModel *m_model;
};

void Test_NativeInfoWidget::SetUp()
{
    m_model = new dcc::systeminfo::SystemInfoModel;
    m_widget = new dccV20::systeminfo::NativeInfoWidget(m_model);
}

void Test_NativeInfoWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;

    delete m_model;
    m_model = nullptr;
}

TEST_F(Test_NativeInfoWidget, full)
{
    EXPECT_NO_THROW(m_widget->setProductName("test"));
    EXPECT_NO_THROW(m_widget->setProcessor("test"));
    EXPECT_NO_THROW(m_widget->setVersionNumber("test"));
    EXPECT_NO_THROW(m_widget->setType("test"));
    EXPECT_NO_THROW(m_widget->setProcessor("test"));
    EXPECT_NO_THROW(m_widget->setMemory("test"));
    EXPECT_NO_THROW(m_widget->setLicenseState(ActiveState::Authorized));
}
