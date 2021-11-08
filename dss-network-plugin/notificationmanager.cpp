/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "notificationmanager.h"
#include "notification/bubblemanager.h"
static QStringList s_list;
static QVariantMap s_map;
static BubbleManager *s_bubbleManager = nullptr;

uint NotificationManager::Notify(QString icon, QString summary, QString body)
{
    if (!s_bubbleManager) {
        s_bubbleManager = new BubbleManager();
    }
    return s_bubbleManager->Notify("dde-control-center", 0, icon, summary, body);
}
