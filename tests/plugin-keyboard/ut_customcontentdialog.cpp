#include "gtest/gtest.h"
#include "src/plugin-keyboard/operation/shortcutmodel.h"
#include "src/plugin-keyboard/window/customcontentdialog.h"

#include <QPushButton>
#include <QSignalSpy>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_CustomContentDialog : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    CustomContentDialog *dialog = nullptr;
    ShortcutModel *model = nullptr;
};

void Tst_CustomContentDialog::SetUp()
{
    model = new ShortcutModel();
    dialog = new CustomContentDialog(model);
}

void Tst_CustomContentDialog::TearDown()
{
    delete model;
    model = nullptr;
    delete dialog;
    dialog = nullptr;
}

TEST_F(Tst_CustomContentDialog, emitSignal) { }
