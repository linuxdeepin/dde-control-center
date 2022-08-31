// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
