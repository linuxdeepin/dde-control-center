#include "shortcutkey.h"

#include "gtest/gtest.h"

class Tst_ShortcutKey : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    ShortcutKey *key = nullptr;
};

void Tst_ShortcutKey::SetUp()
{
    key = new ShortcutKey();
}

void Tst_ShortcutKey::TearDown()
{
    delete key;
    key = nullptr;
}

TEST_F(Tst_ShortcutKey, key)
{
    QStringList ls = { "aaa", "bbb", "ccc"};

    EXPECT_NO_THROW(key->setTextList(ls));
}
