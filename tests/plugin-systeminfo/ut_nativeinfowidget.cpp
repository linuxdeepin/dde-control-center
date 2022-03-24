#include "dtkcore_global.h"
#include "dtkwidget_global.h"
#include "interface/namespace.h"
#include "src/plugin-systeminfo/operation/systeminfomodel.h"

#define private public
#include "widgets/titlevalueitem.h"
#include "src/plugin-systeminfo/window/nativeinfowidget.h"
#undef private

#include <DListView>
#include <DToolButton>
#include <DSysInfo>
#include <QSignalSpy>
#include <QSettings>
#include <QScopedPointer>
#include <QTest>
#include <gtest/gtest.h>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class Test_HostNameItem : public testing::Test
{
public:
    virtual void SetUp() override;
    virtual void TearDown() override;

public:
    HostNameItem *m_item;
};

void Test_HostNameItem::SetUp()
{
    m_item = new HostNameItem;
}

void Test_HostNameItem::TearDown()
{
    delete m_item;
    m_item = nullptr;
}

class Test_NativeInfoWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    NativeInfoWidget *m_widget = nullptr;
    SystemInfoModel *m_model;
};

void Test_NativeInfoWidget::SetUp()
{
    m_model = new SystemInfoModel;
    m_model->setHostName("swq-pc322");
    m_model->setKernel("4.19.0-amd64-desktop");
    m_model->setLicenseState(Authorized);
    m_model->setMemory(10000,8000);
    m_model->setProcessor("Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz333");
    m_model->setProductName("统信桌面操作系统");
    m_model->setType(64);
    m_model->setVersion("专业版 (1050)");
    m_model->setVersionNumber("2022");
    m_widget = new NativeInfoWidget(m_model);
}

void Test_NativeInfoWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;

    delete m_model;
    m_model = nullptr;
}

TEST_F(Test_HostNameItem, method)
{
    m_item->setHostName("Test");
    EXPECT_EQ("Test", m_item->m_hostNameLabel->text());

    m_item->onSetError("Error");
    EXPECT_EQ("Error", m_item->m_alertMessage);

    m_item->setFixedWidth(300);
    const QString &str = m_item->getElidedText("Test");
    EXPECT_EQ(str, "Test");

    DTK_WIDGET_NAMESPACE::DToolButton *toolBtn = m_item->m_hostNameBtn;
    QSignalSpy spy1(toolBtn, SIGNAL(clicked()));
    toolBtn->clicked();
    EXPECT_EQ(spy1.count(), 1);
}

TEST_F(Test_HostNameItem, lineEdit)
{
    HostNameEdit* edit = m_item->m_hostNameLineEdit;
    QSignalSpy spy1(edit, SIGNAL(textEdited(const QString &)));
    edit->textEdited("testsss");
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(edit, SIGNAL(focusChanged(bool)));
    edit->focusChanged(false);
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(edit->lineEdit(), SIGNAL(editingFinished()));
    edit->lineEdit()->editingFinished();
    EXPECT_EQ(spy3.count(), 1);

    m_item->m_hostNameLineEdit->lineEdit()->setText("--test--");
    edit->lineEdit()->editingFinished();
    edit->editingFinished();
    EXPECT_EQ(spy3.count(), 2);

    QSignalSpy spy4(edit, SIGNAL(alertChanged(bool)));
    edit->alertChanged(true);
    EXPECT_EQ(spy4.count(), 1);

    m_item->m_hostNameLineEdit->setText("test");
    QString text =  m_item->m_hostNameLineEdit->text();
    EXPECT_EQ(text, "test");
}

TEST_F(Test_NativeInfoWidget, Method){

    QString copyrightVale = m_widget->systemCopyright();
    QString oem_copyright;
    if (DTK_CORE_NAMESPACE::DSysInfo::isCommunityEdition())
        oem_copyright = QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2011-%1 Deepin Community").arg(2022);
    else
        oem_copyright = QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2019-%1 UnionTech Software Technology Co., LTD").arg(2022);
    EXPECT_EQ(copyrightVale, oem_copyright);
}

TEST_F(Test_NativeInfoWidget, LicenseState){

    QString text;
    QScopedPointer<TitleAuthorizedItem> authorized(new TitleAuthorizedItem);

    m_widget->setLicenseState(authorized.data(), Authorized);
    text =  authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Activated"));

    m_widget->setLicenseState(authorized.data(), Unauthorized);
    text = authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("To be activated"));

    m_widget->setLicenseState(authorized.data(), AuthorizedLapse);
    text = authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Expired"));

    m_widget->setLicenseState(authorized.data(), TrialAuthorized);
    text = authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("In trial period"));

    m_widget->setLicenseState(authorized.data(), TrialExpired);
    text = authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Trial expired"));
}

