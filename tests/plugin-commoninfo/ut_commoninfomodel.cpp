#include "interface/namespace.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DCC_USE_NAMESPACE
class Test_CommonInfoModel: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    CommonInfoModel *m_model = nullptr;
};

void Test_CommonInfoModel::SetUp()
{
    m_model = new CommonInfoModel();
}

void Test_CommonInfoModel::TearDown()
{
    delete m_model;
    m_model = nullptr;
}


TEST_F(Test_CommonInfoModel, setEntryLists)
{
    QSignalSpy spy1(m_model, SIGNAL(entryListsChanged(const QStringList&)));
    QStringList list;
    list << "备份" << "还原";
    m_model->setEntryLists(list);
    EXPECT_EQ(spy1.count(), 1);
    QStringList value = m_model->entryLists();
    EXPECT_EQ(value, list);
}

TEST_F(Test_CommonInfoModel, setThemeEnabled)
{
    QSignalSpy spy1(m_model, SIGNAL(themeEnabledChanged(const bool)));
    m_model->setThemeEnabled(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->themeEnabled();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setShowGrubEditAuth)
{
    m_model->setShowGrubEditAuth(true);
    bool value = m_model->isShowGrubEditAuth();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setGrubEditAuthEnabled)
{
    QSignalSpy spy1(m_model, SIGNAL(grubEditAuthEnabledChanged(const bool)));
    m_model->setGrubEditAuthEnabled(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->grubEditAuthEnabled();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setDefaultEntry)
{
    QSignalSpy spy1(m_model, SIGNAL(defaultEntryChanged(const QString&)));
    m_model->setDefaultEntry("uos-v23");
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->defaultEntry();
    EXPECT_EQ(value, "uos-v23");
}

TEST_F(Test_CommonInfoModel, setUpdating)
{
    m_model->setUpdating(false);
    QSignalSpy spy1(m_model, SIGNAL(updatingChanged(const bool)));
    m_model->setUpdating(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->updating();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setUeProgram)
{
    m_model->setUeProgram(false);
    QSignalSpy spy1(m_model, SIGNAL(ueProgramChanged(const bool)));
    m_model->setUeProgram(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->ueProgram();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setDeveloperModeState)
{
    QSignalSpy spy1(m_model, SIGNAL(developerModeStateChanged(const bool)));
    m_model->setDeveloperModeState(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->developerModeState();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setIsLogin)
{
    QSignalSpy spy1(m_model, SIGNAL(isLoginChenged(const bool)));
    m_model->setIsLogin(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->isLogin();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setBootDelay)
{
    m_model->setBootDelay(false);
    QSignalSpy spy1(m_model, SIGNAL(bootDelayChanged(const bool)));
    m_model->setBootDelay(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->bootDelay();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setActivation)
{
    m_model->setActivation(false);
    QSignalSpy spy1(m_model, SIGNAL(LicenseStateChanged(const bool)));
    m_model->setActivation(true);
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->isActivate();
    EXPECT_EQ(value, true);
}

TEST_F(Test_CommonInfoModel, setBackground)
{
    QSignalSpy spy1(m_model, SIGNAL(backgroundChanged(const QPixmap&)));
    m_model->setBackground(QPixmap());
    EXPECT_EQ(spy1.count(), 1);
    auto value = m_model->background();
    EXPECT_EQ(value, QPixmap());
}










