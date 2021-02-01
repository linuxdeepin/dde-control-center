#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/separator.h"

using namespace dcc::widgets;

class Tst_HSeparator : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new HSeparator();
        obj2 = new VSeparator();
    }

    void TearDown() override
    {
        delete obj;
        delete obj2;
        obj = nullptr;
        obj2 = nullptr;
    }

public:
    HSeparator *obj = nullptr;
    VSeparator *obj2 = nullptr;
};

TEST_F(Tst_HSeparator, coverage)
{

}
