/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "datetimecombox.h"
#include <QDebug>

using namespace DCC_NAMESPACE::datetime;

datetimeCombox::datetimeCombox(QWidget *parent)
    : QComboBox(parent)
{
    /*以下三行为默认程序模块服务，由于每个cpp只能有一种翻译，故将注释分配到其他地方*/
    //~ contents_path /defapp/Music/Add Application
    //~ child_page Music
    tr("Add Application");
}

//For different with user operate or default value
void datetimeCombox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    Q_EMIT click();

    QComboBox::mousePressEvent(event);
}
