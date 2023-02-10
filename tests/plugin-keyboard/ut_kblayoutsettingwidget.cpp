//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "src/plugin-keyboard/window/kblayoutsettingwidget.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"

#include <QSignalSpy>
#include <DCommandLinkButton>
#include <QModelIndex>

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_KBLayoutSettingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    KBLayoutSettingWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_KBLayoutSettingWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new KBLayoutSettingWidget();
}

void Tst_KBLayoutSettingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_KBLayoutSettingWidget, emitSignal)
{
    widget->setModel(model);

    DCommandLinkButton *addLayout = widget->findChild<DCommandLinkButton *>("AddLayout");
    ASSERT_TRUE(nullptr != addLayout);
    QSignalSpy spyOnAddKeyBoard(widget, SIGNAL(layoutAdded(const QStringList &)));
    addLayout->clicked(true);
    EXPECT_EQ(spyOnAddKeyBoard.count(), 1);

    KBLayoutListView *myListView = widget->findChild<KBLayoutListView *>("KbLayoutListView");
    ASSERT_TRUE(nullptr != myListView);

    widget->onAddKeyboard("cn","汉语");
    widget->onAddKeyboard("cn;altgr-pinyin","Hanyu Pinyin (altgr)");
    widget->onDefault("汉语");
    DCommandLinkButton *m_editKBLayout = widget->findChild<DCommandLinkButton *>("Edit");
    m_editKBLayout->click();
    m_editKBLayout->click();
    DListView *m_kbLayoutListView = widget->findChild<DListView *>("KbLayoutListView");
    QStandardItemModel *m_kbLayoutModel = widget->findChild<QStandardItemModel *>("KbLayoutModel");
    m_kbLayoutListView->clicked(m_kbLayoutModel->index(0, 0));
    m_kbLayoutListView->clicked(m_kbLayoutModel->index(1, 0));
    EXPECT_EQ(m_kbLayoutListView->count(), 2 + 1);
}
