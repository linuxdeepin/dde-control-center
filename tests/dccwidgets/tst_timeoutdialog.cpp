#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/timeoutdialog.h"

class Tst_TimeoutDialog : public testing::Test
{
public:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }

public:

};

TEST_F(Tst_TimeoutDialog, coverage)
{
    TimeoutDialog *obj = new TimeoutDialog(3);
    obj->setMessageModel("aaa");
    obj->messageModel();
}
