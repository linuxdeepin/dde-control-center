/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unionidbindreminderdialog.h"
#include <QPushButton>
#include <DDBusSender>

DCORE_USE_NAMESPACE

UnionIDBindReminderDialog::UnionIDBindReminderDialog(QWidget *parent)
    : DDialog(tr("The user account is not linked to Union ID"),
              tr("To reset passwords, you should authenticate your Union ID first. Click \"Go to Link\" to finish the settings."))
{
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

