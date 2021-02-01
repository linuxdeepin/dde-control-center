#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/filechoosewidget.h"

using namespace dcc::widgets;

class Tst_FileChooseWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new FileChooseWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    FileChooseWidget *obj = nullptr;
};

TEST_F(Tst_FileChooseWidget, coverage)
{
    obj->setTitle("aaa");
    obj->setIsErr(false);
    obj->edit();
}
