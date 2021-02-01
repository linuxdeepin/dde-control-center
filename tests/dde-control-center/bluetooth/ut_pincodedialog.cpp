#include "../src/frame/modules/bluetooth/pincodedialog.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dcc::bluetooth;

class Tst_PinCodeDialog : public testing::Test
{
public:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }
};

TEST_F(Tst_PinCodeDialog, FullTest)
{
    PinCodeDialog *dlg = PinCodeDialog::instance("pincode", true);
    EXPECT_EQ(dlg->pinCode(), "pincode");

    PinCodeDialog *dlg1 = PinCodeDialog::instance("pincode", true);
    dlg1->setPinCode("");

    QMetaObject::invokeMethod(dlg, "deleteLater", Qt::DirectConnection);
    dlg = nullptr;
}
