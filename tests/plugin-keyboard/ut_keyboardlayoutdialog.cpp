#include "gtest/gtest.h"
#include "src/plugin-keyboard/window/keyboardlayoutdialog.h"

#include <QPushButton>
#include <QSignalSpy>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_KeyboardLayoutDialog : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    KeyboardLayoutDialog *dialog = nullptr;
};

void Tst_KeyboardLayoutDialog::SetUp()
{
    dialog = new KeyboardLayoutDialog();
}

void Tst_KeyboardLayoutDialog::TearDown()
{
    delete dialog;
    dialog = nullptr;
}

TEST_F(Tst_KeyboardLayoutDialog, emitSignal) { }
