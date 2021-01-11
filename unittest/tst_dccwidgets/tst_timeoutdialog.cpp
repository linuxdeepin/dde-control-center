#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/timeoutdialog.h"

class Tst_TimeoutDialog : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TimeoutDialog(1);
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TimeoutDialog *obj = nullptr;
};

TEST_F(Tst_TimeoutDialog, coverage)
{
    obj->setMessageModel("aaa");
    obj->messageModel();
    obj->exec();
    obj->open();
}
