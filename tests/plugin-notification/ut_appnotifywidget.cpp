//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "src/plugin-notification/window/appnotifywidget.h"
#include "src/plugin-notification/window/notificationitem.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "src/plugin-notification/operation/model/appitemmodel.h"
#include "interface/namespace.h"

#include <DSwitchButton>

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_AppNotifyWidget : public testing::Test
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

TEST_F(Tst_AppNotifyWidget, init)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    AppNotifyWidget widget(model.getAppModel(0));
}

TEST_F(Tst_AppNotifyWidget, FullTest)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    AppNotifyWidget widget(model.getAppModel(0));

    AppItemModel *appmodel = model.getAppModel(0);
    appmodel->allowNotifyChanged(true);
    appmodel->lockShowNotifyChanged(true);
    appmodel->notifySoundChanged(true);
    appmodel->showNotifyPreviewChanged(true);
    appmodel->showInNotifyCenterChanged(true);

    QSignalSpy spy(&widget, SIGNAL(requestSetAppSetting(const QString &, uint, QVariant)));

    DSwitchButton *button = widget.findChild<DSwitchButton *>();
    button->checkedChanged(!button->isChecked());

    NotificationItem *item1 = widget.findChild<NotificationItem *>("NotifySound");
    NotificationItem *item2 = widget.findChild<NotificationItem *>("LockShowNotify");
    NotificationItem *item3 = widget.findChild<NotificationItem *>("ShowInNotifyCenter");
    NotificationItem *item4 = widget.findChild<NotificationItem *>("ShowNotifyPreview");

    item1->stateChanged(!item1->getState());
    item2->stateChanged(!item2->getState());
    item3->stateChanged(!item3->getState());
    item4->stateChanged(!item4->getState());

    EXPECT_GT(spy.count(), 5);

    widget.setModel(model.getAppModel(1));
}
