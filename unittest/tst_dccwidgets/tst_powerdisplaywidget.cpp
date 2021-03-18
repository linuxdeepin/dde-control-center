#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/powerdisplaywidget.h"

using namespace dcc::widgets;

class Tst_PowerDisplayWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new PowerDisplayWidget("aaa");
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    PowerDisplayWidget *obj = nullptr;
};

TEST_F(Tst_PowerDisplayWidget, coverage)
{
    obj->setText("aaa");
    obj->setTitle("bbb");
}
