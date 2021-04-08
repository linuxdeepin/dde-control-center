#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/nextbutton.h"

using namespace dcc::widgets;

class Tst_NextButton : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new NextButton;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    NextButton *obj = nullptr;
};

TEST_F(Tst_NextButton, coverage)
{

}
