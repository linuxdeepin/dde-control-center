#include <gtest/gtest.h>

#include "widgets/settingsheaderitem.h"

#include <QStandardItemModel>

using namespace DCC_NAMESPACE;

class Tst_SettingsHeaderItem : public testing::Test
{
public:
    void SetUp() override { obj = new SettingsHeaderItem(); }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SettingsHeaderItem *obj = nullptr;
};

TEST_F(Tst_SettingsHeaderItem, coverage)
{
    obj->setTitle("aaa");
    obj->textLabel();
    obj->layout();
    obj->setRightWidget(new QWidget);
}
