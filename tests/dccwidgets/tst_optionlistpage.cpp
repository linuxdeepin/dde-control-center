#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/optionlistpage.h"

using namespace dcc::widgets;

class Tst_OptionListPage : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new OptionListPage(QStringList() << "aaa" << "bbb");
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    OptionListPage *obj = nullptr;
};

TEST_F(Tst_OptionListPage, coverage)
{

}
