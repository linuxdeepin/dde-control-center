/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"
#include "interface/moduleobject.h"

#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

DCC_USE_NAMESPACE

QByteArray DCC_NAMESPACE::getFileMd5(const QString &filePath)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (true) {
        if (bytesToWrite > 0) {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        } else {
            break;
        }

        if (bytesWritten == totalBytes) {
            break;
        }
    }

    localFile.close();
    return ch.result();
}

ModuleObject *DCC_NAMESPACE::GetModuleByUrl(ModuleObject *const root, const QString &url)
{
    ModuleObject *obj = root;
    ModuleObject *parent = nullptr;
    QStringList names = url.split('/');
    while (!names.isEmpty() && obj) {
        const QString &name = names.takeFirst();
        parent = obj;
        obj = nullptr;
        for (auto &&child : parent->childrens()) {
            if (child->name() == name) {
                obj = child;
                break;
            }
        }
    }
    return names.isEmpty() ? obj : nullptr;
}

QString DCC_NAMESPACE::GetUrlByModule(ModuleObject *const module)
{
    QStringList url;
    ModuleObject *obj = module;
    while (obj && obj->getParent()) {
        url.prepend(obj->name());
        obj = obj->getParent();
    }
    return url.join('/');
}
