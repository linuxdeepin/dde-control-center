#include "src/plugin-notification/window/systemnotifywidget.h"
#include "src/plugin-notification/window/notificationitem.h"
#include "src/plugin-notification/window/timeslotitem.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "src/plugin-notification/operation/model/sysitemmodel.h"
#include "interface/namespace.h"
#include "widgets/switchwidget.h"

#include <DSwitchButton>

#include <QSignalSpy>

#include <gtest/gtest.h>

DCC_USE_NAMESPACE
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
