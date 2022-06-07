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

#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

QByteArray getFileMd5(const QString &filePath)
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
        if(bytesToWrite > 0) {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        } else {
            break;
        }

        if(bytesWritten == totalBytes) {
            break;
        }
    }

    localFile.close();
    return ch.result();
}
