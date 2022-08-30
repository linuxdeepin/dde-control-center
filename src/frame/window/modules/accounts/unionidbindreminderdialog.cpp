// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "unionidbindreminderdialog.h"
#include <QPushButton>
#include <DDBusSender>

DCORE_USE_NAMESPACE

UnionIDBindReminderDialog::UnionIDBindReminderDialog(QWidget *parent)
    : DDialog(tr("The user account is not linked to UOS ID"),
              tr("To reset passwords, you should authenticate your UOS ID first. Click \"Go to Link\" to finish the settings."))
{
    setFixedWidth(380);
    setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
    setParent(parent);
    setIcon(QIcon::fromTheme("dialog-warning"));
    QStringList buttons;
    buttons << tr("Cancel");
    addButtons(buttons);
    addButton(tr("Go to Link"), true, ButtonRecommend);

    connect(getButton(1), &QPushButton::clicked, this, []{
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method("ShowPage")
        .arg(QStringLiteral("cloudsync"))
        .arg(tr(""))
        .call();
    });
}

