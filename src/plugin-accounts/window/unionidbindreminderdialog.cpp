//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "unionidbindreminderdialog.h"
#include <QPushButton>
#include <DDBusSender>
#include <DIconTheme>

DCORE_USE_NAMESPACE
DGUI_USE_NAMESPACE

UnionIDBindReminderDialog::UnionIDBindReminderDialog(QWidget *parent)
    : DDialog(tr("The user account is not linked to Union ID"),
              tr("To reset passwords, you should authenticate your Union ID first. Click \"Go to Link\" to finish the settings."))
{
    setParent(parent);
    setIcon(DIconTheme::findQIcon("dialog-warning"));
    QStringList buttons;
    buttons << tr("Cancel");
    addButtons(buttons);
    addButton(tr("Go to Link"), true, ButtonRecommend);

    connect(getButton(1), &QPushButton::clicked, this, []{
        DDBusSender()
        .service("org.deepin.dde.ControlCenter1")
        .interface("org.deepin.dde.ControlCenter1")
        .path("/org/deepin/dde/ControlCenter1")
        .method("ShowPage")
        .arg(QStringLiteral("cloudsync"))
        .arg(tr(""))
        .call();
    });
}

