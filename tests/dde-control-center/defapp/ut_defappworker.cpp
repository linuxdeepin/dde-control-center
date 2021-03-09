#include "../src/frame/window/modules/defapp/defappwidget.h"
#include "../src/frame/modules/defapp/defappworker.h"
#include "../src/frame/modules/defapp/defappmodel.h"
#include "interface/namespace.h"

#include <DListView>
#include <DFloatingButton>
#include <QSignalSpy>
#include <gtest/gtest.h>

using namespace DCC_NAMESPACE::defapp;
using namespace dcc::defapp;
DWIDGET_USE_NAMESPACE

class Test_DefappWorker : public testing::Test
{
public:
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void Test_DefappWorker::SetUp()
{
}

void Test_DefappWorker::TearDown()
{
}

TEST_F(Test_DefappWorker, FullTest)
{
    DefAppModel model;
    DefAppWorker worker(&model);
    worker.active();
    Mime *dbus = worker.getDbusObject();
    dbus->Change();
    dbus->GetDefaultApp("x-scheme-handler/http");
    dbus->ListApps("x-scheme-handler/http");
    dbus->ListUserApps("x-scheme-handler/http");

    QStringList mimelist;
    mimelist << "x-scheme-handler/http"
             << "x-scheme-handler/ftp"
             << "x-scheme-handler/https"
             << "text/html"
             << "text/xml"
             << "text/xhtml_xml"
             << "text/xhtml+xml";
    dbus->SetDefaultApp(mimelist, "uos-browser.desktop");
    dbus->DeleteApp(mimelist, "uos-browser.desktop");
    dbus->AddUserApp(mimelist, "deepin-custom-test.desktop");
    dbus->DeleteUserApp("uos-browser.desktop");

    worker.deactive();
}
