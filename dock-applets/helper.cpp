/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "helper.h"

#include <QProcess>
#include <QFile>

ExternalObject::ExternalObject(QObject *parent)
    :QObject(parent)
{

}

QString ExternalObject::getIconUrl(QString path)
{
    QString iconPath = "file://"DOCKAPPLETSDIR"/icons/" + path;
    return iconPath;
}

void ExternalObject::xdgOpen(QString path)
{
    QProcess::execute("xdg-open " + path);
}

bool ExternalObject::isPathExist(QString path)
{
    if(path.startsWith("file://")){
        path = path.replace("file://", "");
    }
    return QFile::exists(path);
}
