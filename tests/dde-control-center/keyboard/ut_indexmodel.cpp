#include "indexmodel.h"

#include <QDebug>

#include "gtest/gtest.h"

#include <iostream>

using namespace dcc::keyboard;

DWIDGET_USE_NAMESPACE

class Tst_IndexModel : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
//    GeneralKBSettingWidget *widget = nullptr;
    IndexModel *model = nullptr;
};

void Tst_IndexModel::SetUp()
{
    model = new IndexModel();
}

void Tst_IndexModel::TearDown()
{
    delete model;
    model = nullptr;
}

TEST_F(Tst_IndexModel, model)
{
    QList<QString> ls = { "a", "b", "c"};

    EXPECT_NO_THROW(model->setLetters(ls));
    EXPECT_EQ(ls, model->letters());
}

TEST_F(Tst_IndexModel, testOperator)
{
    MetaData md("a");
    EXPECT_NO_THROW(md.setPinyin("a1"));
    MetaData md1("a");
    EXPECT_NO_THROW(md1.setPinyin("a2"));
    EXPECT_TRUE(md1 == md);
    EXPECT_TRUE(md1 > md);
}

TEST_F(Tst_IndexModel, metaData)
{
    QList<MetaData> lmd = { MetaData("a"), MetaData("b"), MetaData("c") };
    EXPECT_NO_THROW(lmd[1].setKey("key1"));
    EXPECT_EQ(lmd[1].key(), "key1");
    EXPECT_NO_THROW(lmd[1].setPinyin("pinyin1"));
    EXPECT_EQ(lmd[1].pinyin(), "pinyin1");
    EXPECT_NO_THROW(lmd[1].setText("text1"));
    EXPECT_EQ(lmd[1].text(), "text1");
    EXPECT_NO_THROW(lmd[1].setSection(true));
    EXPECT_TRUE(lmd[1].section());
    EXPECT_NO_THROW(lmd[1].setSelected(true));
    EXPECT_TRUE(lmd[1].selected());

    EXPECT_NO_THROW(model->setMetaData(lmd));
    EXPECT_EQ(lmd, model->metaData());
    EXPECT_EQ(3, model->getModelCount());
    EXPECT_EQ(1, model->indexOf(lmd[1]));
}
