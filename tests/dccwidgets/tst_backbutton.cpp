#include <gtest/gtest.h>

#include "../../include/widgets/backbutton.h"

using namespace dcc::widgets;

class Tst_BackButtbon : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BackButton;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BackButton *obj = nullptr;
};
