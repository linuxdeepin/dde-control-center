#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/tipsitem.h"

using namespace dcc::widgets;

class Tst_TipsItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TipsItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TipsItem *obj = nullptr;
};

TEST_F(Tst_TipsItem, coverage)
{
    obj->setText("aaa");
}
