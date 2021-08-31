
#define private public
#include "../src/frame/window/modules/systeminfo/nativeinfowidget.h"
#include "../src/frame/modules/systeminfo/systeminfomodel.h"
#undef private

#include <DListView>
#include <DToolButton>
#include <QSignalSpy>
#include <qsettings.h>
#include <QTest>
#include <gtest/gtest.h>

using namespace dccV20::systeminfo;
using namespace dcc::systeminfo;

DWIDGET_USE_NAMESPACE

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
    m_model = new dcc::systeminfo::SystemInfoModel;
    m_model->setBootDelay(false);
    m_model->setDefaultEntry("");
    m_model->setDisk(83.5);
    m_model->setHostName("swq-pc322");
    m_model->setKernel("4.19.0-amd64-desktop");
    m_model->setLicenseState(dcc::systeminfo::Authorized);
    m_model->setMemory(16,15.6);
    m_model->setProcessor("Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz333");
    m_model->setProductName("统信桌面操作系统");
    m_model->setThemeEnabled(false);
    m_model->setType(64);
    m_model->setUpdating(false);
    m_model->setVersion("专业版 (1040)");
    m_model->setVersionNumber("2011");
    m_widget = new dccV20::systeminfo::NativeInfoWidget(m_model);
}

void Test_NativeInfoWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;

    delete m_model;
    m_model = nullptr;
}

TEST_F(Test_NativeInfoWidget, full)
{
    QSignalSpy spy1(m_widget, SIGNAL(clickedActivator()));
    m_widget->m_authorized->clicked();
    EXPECT_EQ(spy1.count(), 1);

    DTK_WIDGET_NAMESPACE::DToolButton* btn = m_widget->m_hostNameBtn;
    QSignalSpy spy2(btn, SIGNAL(clicked()));
    btn->clicked(true);
    EXPECT_EQ(spy2.count(), 1);

    m_widget->showMaximized();
    m_widget->showNormal();
}

TEST_F(Test_NativeInfoWidget, Slots){

    QString text;

    m_widget->setVersionNumber("test");
    text = m_widget->m_versionNumber->value();
    EXPECT_EQ(text, "test");

    m_widget->setType("test");
    text = QObject::tr("%1-bit").arg("test");
    EXPECT_EQ(text, m_widget->m_type->value());

    m_widget->setProcessor("test");
    text = m_widget->m_processor->value();
    EXPECT_EQ(text, "test");

    m_widget->setProductName("test");
    text = m_widget->m_productName->value();
    EXPECT_EQ(text, "test");

    m_widget->setMemory("test");
    text = m_widget->m_memory->value();
    EXPECT_EQ(text, "test");

    m_widget->setEdition("test");
    text = m_widget->m_version->value();
    EXPECT_EQ(text, "test");

}

TEST_F(Test_NativeInfoWidget, SystemInfoModel){

    m_model->hostNameChanged("test");
    QString text = m_widget->m_hostNameLabel->text();
    EXPECT_EQ(text, "test");

    m_model->setHostNameError("test");
    EXPECT_EQ(m_widget->m_alertMessage, "test");

}

TEST_F(Test_NativeInfoWidget, m_hostNameLineEdit){

    HostNameEdit* edit = m_widget->m_hostNameLineEdit;
    QSignalSpy spy1(edit, SIGNAL(textEdited(const QString &)));
    edit->textEdited("testsss");
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(edit, SIGNAL(focusChanged(bool)));
    edit->focusChanged(false);
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(edit->lineEdit(), SIGNAL(editingFinished()));
    edit->lineEdit()->editingFinished();
    EXPECT_EQ(spy3.count(), 1);

    m_widget->m_hostNameLineEdit->lineEdit()->setText("--test--");
    edit->lineEdit()->editingFinished();
    edit->editingFinished();
    EXPECT_EQ(spy3.count(), 2);

    QSignalSpy spy4(edit, SIGNAL(alertChanged(bool)));
    edit->alertChanged(true);
    EXPECT_EQ(spy4.count(), 1);

    m_widget->m_hostNameLineEdit->setText("test");
    QString text =  m_widget->m_hostNameLineEdit->text();
    EXPECT_EQ(text, "test");

}

TEST_F(Test_NativeInfoWidget, Method){

    QString copyrightVale =  m_widget->systemCopyright();
    QString oem_copyright = QString(QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2019-2021 UnionTech Software Technology Co., LTD")).arg(2019);
    EXPECT_EQ(copyrightVale, oem_copyright);

    QString systemLogo = m_widget->systemLogo();
    EXPECT_EQ(systemLogo, ":/systeminfo/themes/dark/icons/logo.png");
}

TEST_F(Test_NativeInfoWidget, LicenseState){

    QString text;

    m_widget->setLicenseState(dcc::systeminfo::Authorized);
    text =  m_widget->m_authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Activated"));

    m_widget->setLicenseState(dcc::systeminfo::Unauthorized);
    text = m_widget->m_authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("To be activated"));

    m_widget->setLicenseState(dcc::systeminfo::AuthorizedLapse);
    text = m_widget->m_authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Expired"));

    m_widget->setLicenseState(dcc::systeminfo::TrialAuthorized);
    text = m_widget->m_authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("In trial period"));

    m_widget->setLicenseState(dcc::systeminfo::TrialExpired);
    text = m_widget->m_authorized->m_value->text();
    EXPECT_EQ(text, QObject::tr("Trial expired"));
}

