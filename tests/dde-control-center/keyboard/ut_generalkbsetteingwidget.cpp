#include "generalkbsettingwidget.h"
#include "keyboardmodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"

#include <QSignalSpy>
#include <QDebug>

#include "gtest/gtest.h"

#include <iostream>

using namespace dccV20::keyboard;
using namespace dcc::keyboard;
using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

class Tst_GeneralKbSetteingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    GeneralKBSettingWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_GeneralKbSetteingWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new GeneralKBSettingWidget(model);
}

void Tst_GeneralKbSetteingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_GeneralKbSetteingWidget, requestKBDelayChanged)
{
    TitledSliderItem *repeatDealy = widget->findChild<TitledSliderItem * >("RepeatDelay");
    ASSERT_TRUE(repeatDealy != nullptr);
    QSignalSpy spy1(widget, SIGNAL(requestKBDelayChanged(const int)));
    repeatDealy->slider()->valueChanged(3);
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Tst_GeneralKbSetteingWidget, requestKBSpeedChanged)
{
    TitledSliderItem *repeatRate = widget->findChild<TitledSliderItem * >("RepeatRate");
    ASSERT_TRUE(repeatRate != nullptr);
    QSignalSpy spy2(widget, SIGNAL(requestKBSpeedChanged(const int)));
    repeatRate->slider()->valueChanged(3);
    EXPECT_EQ(spy2.count(), 1);
}

TEST_F(Tst_GeneralKbSetteingWidget, requestNumLockChanged)
{
    SwitchWidget *numLock = widget->findChild<SwitchWidget *>("NumLock");
    ASSERT_TRUE(numLock != nullptr);
    QSignalSpy spyNumLock(widget, SIGNAL(requestNumLockChanged(const bool)));
    numLock->checkedChanged(numLock->checked());
    EXPECT_EQ(spyNumLock.count(), 1);
}

TEST_F(Tst_GeneralKbSetteingWidget, requestCapsLockChanged)
{
    SwitchWidget *upper = widget->findChild<SwitchWidget *>("Upper");
    ASSERT_TRUE(upper != nullptr);
    QSignalSpy spyUpper(widget, SIGNAL(requestCapsLockChanged(const bool)));
    upper->checkedChanged(upper->checked());
    EXPECT_EQ(spyUpper.count(), 1);
}

TEST_F(Tst_GeneralKbSetteingWidget, model)
{
    QMap<QStringList, int> allShortcuts;
    QStringList strListShortcut;
    QMap<QString, QString> m_userLayout;
    QString str("zh_CN.UTF-8");
    QStringList strList;
    strList.append(str);
    strListShortcut.append("Alt");
    strListShortcut.append("F10");
    allShortcuts.insert(strListShortcut, 4);
    m_userLayout.insert("cn","汉语");
    EXPECT_NO_THROW(model->setLang(str));
    EXPECT_NO_THROW(model->setNumLock(false));
    EXPECT_EQ(model->numLock(), false);
    EXPECT_NO_THROW(model->setCapsLock(true));
    EXPECT_EQ(model->capsLock(), true);
    EXPECT_NO_THROW(model->setLangChangedState(0));
    EXPECT_EQ(model->getLangChangedState(), 0);
    EXPECT_NO_THROW(model->setRepeatDelay(7));
    EXPECT_EQ(model->repeatDelay(), 7);
    EXPECT_NO_THROW(model->setRepeatInterval(1));
    EXPECT_EQ(model->repeatInterval(),1);
    EXPECT_NO_THROW(model->addUserLayout("cn","汉语"));
    EXPECT_EQ(model->userLayout(),m_userLayout);
    EXPECT_NO_THROW(model->setLayout("汉语"));
    EXPECT_EQ(model->curLayout(),"汉语");
    EXPECT_NO_THROW(model->setLayoutLists(m_userLayout));
    EXPECT_NO_THROW(model->setLocaleLang(strList));
    EXPECT_NO_THROW(model->langFromText(""));
    EXPECT_NO_THROW(model->setAllShortcut(allShortcuts));
    EXPECT_NO_THROW(model->kbLayout());
    EXPECT_NO_THROW(model->allShortcut());
    EXPECT_NO_THROW(model->cleanUserLayout());

}
