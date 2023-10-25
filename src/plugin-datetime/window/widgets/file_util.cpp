// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "file_util.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextCodec>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace installer {

QDir ConcateDir(const QDir &parent_dir, const QString &folder_name)
{
    if (!parent_dir.exists(folder_name)) {
        // TODO(xushaohua): Handles permission error.
        parent_dir.mkpath(folder_name);
    }
    return QDir(parent_dir.filePath(folder_name));
}

bool CopyFile(const QString &src_file, const QString &dest_file, bool overwrite)
{
    QFile dest(dest_file);
    if (dest.exists()) {
        if (overwrite) {
            if (!dest.remove()) {
                qCritical() << "Failed to remove:" << dest_file;
                return false;
            }
        } else {
            qCritical() << dest_file << "exists but is not overwritten";
            return false;
        }
    }
    return QFile::copy(src_file, dest_file);
}

bool CopyFolder(const QString &src_dir, const QString &dest_dir, bool recursive)
{
    QDirIterator::IteratorFlag iter_flag;
    if (recursive) {
        iter_flag = QDirIterator::Subdirectories;
    } else {
        iter_flag = QDirIterator::NoIteratorFlags;
    }
    QDirIterator iter(src_dir, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, iter_flag);
    QFileInfo src_info;
    QString dest_filepath;
    bool ok = true;
    if (!QDir(dest_dir).exists()) {
        ok = CreateDirs(dest_dir);
    }

    // Walk through source folder.
    while (ok && iter.hasNext()) {
        src_info = iter.next();
        dest_filepath = iter.filePath().replace(src_dir, dest_dir);
        if (src_info.isDir()) {
            if (!QDir(dest_filepath).exists()) {
                ok = CreateDirs(dest_filepath);
            }
            if (ok) {
                ok = CopyMode(iter.filePath().toStdString().c_str(),
                              dest_filepath.toStdString().c_str());
            }
        } else if (src_info.isFile()) {
            if (QFile::exists(dest_filepath)) {
                // Remove old file first.
                QFile::remove(dest_filepath);
            }
            ok = QFile::copy(iter.filePath(), dest_filepath);
            if (ok) {
                ok = CopyMode(iter.filePath().toStdString().c_str(),
                              dest_filepath.toStdString().c_str());
            }
        } else if (src_info.isSymLink()) {
            if (QFile::exists(dest_filepath)) {
                QFile::remove(dest_filepath);
            }
            ok = QFile::link(src_info.canonicalFilePath(), dest_filepath);
        } else {
            // Ignores other type of files.
        }
    }
    return ok;
}

bool CopyMode(const char *src_file, const char *dest_file)
{
    struct stat st;
    if (stat(src_file, &st) == -1) {
        return false;
    }

    const mode_t mode = st.st_mode & 0777;
    return (chmod(dest_file, mode) == 0);
}

bool CreateDirs(const QString &dirpath)
{
    return QDir(dirpath).mkpath(".");
}

bool CreateParentDirs(const QString &filepath)
{
    return QFileInfo(filepath).absoluteDir().mkpath(".");
}

QString GetFileBasename(const QString &filepath)
{
    const QString filename = GetFileName(filepath);
    const int dot_index = filename.lastIndexOf(QChar('.'));
    if (dot_index > 0) {
        return filename.left(dot_index);
    } else {
        // Filename does not contain extension name.
        return filename;
    }
}

QString GetFileExtname(const QString &filepath)
{
    const int dot_index = filepath.lastIndexOf(QChar('.'));
    if (dot_index > 0) {
        // Ignores hidden file.
        return filepath.mid(dot_index + 1).toLower();
    }

    return "";
}

QString GetFileName(const QString &filepath)
{
    const int slash_index = filepath.lastIndexOf(QDir::separator());
    if (slash_index > -1) {
        return filepath.mid(slash_index + 1);
    } else {
        // filepath is the filename.
        return filepath;
    }
}

qint64 GetFileSize(const QString &filepath)
{
    QFileInfo info(filepath);
    if (info.exists()) {
        return info.size();
    } else {
        return 0;
    }
}

QString ReadFile(const QString &path)
{
    QFile file(path);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "ReadFile() failed to open" << path;
            return "";
        }
        QTextStream text_stream(&file);
        QString str = text_stream.readAll();
        file.close();
        return str;
    } else {
        qDebug() << "ReadFileContent() file not found: " << path;
        return "";
    }
}

QString ReadGBKFile(const QString &path)
{
    QFile file(path);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "ReadGBKFile() failed to open" << path;
            return "";
        }
        const QByteArray file_data = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("GB18030");
        file.close();
        return codec->toUnicode(file_data);
    } else {
        qDebug() << "ReadGBKFile() file not found:" << path;
        return "";
    }
}

bool ReadRawFile(const QString &path, QByteArray &content)
{
    QFile file(path);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            content = file.readAll();
            return true;
        }
    }
    qDebug() << "ReadRawFile() failed!" << path;
    return false;
}

bool ReadTextFile(const QString &path, QString &content)
{
    QFile file(path);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream text_stream(&file);
            content = text_stream.readAll();
            file.close();
            return true;
        }
    }
    qDebug() << "ReadTextFile() failed!" << path;
    return false;
}

bool WriteTextFile(const QString &path, const QString &content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream text_stream(&file);
        text_stream << content;
        text_stream.flush();
        file.close();
        return true;
    } else {
        qCritical() << "WriteTextFile() failed!"
                    << ", path:" << path;
        return false;
    }
}

} // namespace installer
