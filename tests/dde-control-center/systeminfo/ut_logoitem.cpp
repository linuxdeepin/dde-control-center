#include "../src/frame/modules/systeminfo/logoitem.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::systeminfo;

class Test_LogoItem: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    LogoItem *m_item = nullptr;
};

void Test_LogoItem::SetUp()
{
    m_item = new LogoItem();
}

void Test_LogoItem::TearDown()
{
    delete m_item;
    m_item = nullptr;
}

TEST_F(Test_LogoItem, full)
{
    EXPECT_NO_THROW(m_item->setLogo("logo"));
    EXPECT_NO_THROW(m_item->setDescription("test"));
    EXPECT_NO_THROW(m_item->setDescription(true));

}
