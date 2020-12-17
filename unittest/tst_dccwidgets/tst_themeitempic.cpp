#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/themeitempic.h"

using namespace dcc::widgets;

class Tst_ThemeItemPic : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ThemeItemPic();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ThemeItemPic *obj = nullptr;
};

TEST_F(Tst_ThemeItemPic, coverage)
{
    obj->setPicPath("aaa");
    obj->paintEvent(nullptr);
}
