//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "utils.h"
#include "interface/moduleobject.h"

#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

using namespace DCC_NAMESPACE;

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
