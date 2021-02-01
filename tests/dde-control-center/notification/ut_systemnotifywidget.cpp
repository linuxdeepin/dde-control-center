#include "../src/frame/window/modules/notification/systemnotifywidget.h"
#include "../src/frame/window/modules/notification/notificationitem.h"
#include "../src/frame/window/modules/notification/timeslotitem.h"
#include "../src/frame/modules/notification/notificationworker.h"
#include "../src/frame/modules/notification/model/sysitemmodel.h"
#include "interface/namespace.h"
#include "widgets/switchwidget.h"

#include <DSwitchButton>

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace DCC_NAMESPACE::notification;
using namespace dcc::notification;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

class Tst_SystemNotifyWidget : public testing::Test
{
public:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }

public:

};

TEST_F(Tst_SystemNotifyWidget, init)
{
    NotificationModel model;
    SystemNotifyWidget widget(model.getSystemModel());
}

TEST_F(Tst_SystemNotifyWidget, FullTest)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    SystemNotifyWidget widget(model.getSystemModel());

    SysItemModel *sysmodel = model.getSystemModel();
    sysmodel->disturbModeChanged(true);
    sysmodel->showInDockChanged(true);
    sysmodel->timeSlotChanged(true);
    sysmodel->timeStartChanged("07:00");
    sysmodel->timeEndChanged("18:00");
    sysmodel->lockScreenChanged(true);

    SwitchWidget *sw = widget.findChild<SwitchWidget *>();
    sw->checkedChanged(!sw->checked());

    TimeSlotItem *item = widget.findChild<TimeSlotItem *>();
    item->getTimeStart();
    item->getTimeEnd();

    widget.setModel(model.getSystemModel());
}
