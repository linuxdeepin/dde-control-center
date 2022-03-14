#include <gtest/gtest.h>
#include <QPushButton>
#include "widgets/buttontuple.h"

DCC_USE_NAMESPACE

class Tst_ButtonTuple : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ButtonTuple;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ButtonTuple *obj = nullptr;
};

TEST_F(Tst_ButtonTuple, coverage)
{
    obj->setButtonType(ButtonTuple::ButtonTuple::Save);
    obj->leftButton()->setText("Cancel");
    obj->rightButton()->setText("Ok");
    obj->removeSpacing();
    obj->show();
}
