#include <gtest/gtest.h>

#include "../../include/widgets/basiclistdelegate.h"

using namespace dcc::widgets;

class Tst_BasicListDelegate : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BasicListDelegate;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BasicListDelegate *obj = nullptr;
};
