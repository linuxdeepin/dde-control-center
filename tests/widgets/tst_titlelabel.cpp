#include <gtest/gtest.h>

#include "widgets/titlelabel.h"

#include <QStandardItemModel>

using namespace DCC_NAMESPACE;

class Tst_TitleLabel : public testing::Test
{
public:
    void SetUp() override { obj = new TitleLabel("aaa"); }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TitleLabel *obj = nullptr;
};

TEST_F(Tst_TitleLabel, coverage) { }
