// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INSTALLER_BASE_FILE_UTIL_H
#define INSTALLER_BASE_FILE_UTIL_H

#include <QDir>
#include <QString>

namespace installer {

// Create a folder with |folder_name| in |parent_dir| directory and
// returns a QDir object referencing to its absolute path.
QDir ConcateDir(const QDir &parent_dir, const QString &folder_name);

// Copy file from |src_file| to |dest_file|.
// If |dest_file| exists, overwrite its content if |overwrite| is true, or
// returns false if not overwrite.
bool CopyFile(const QString &src_file, const QString &dest_file, bool overwrite);

// Folder content in |src_dir| into |dest_dir|.
// This method only copy normal files, folders and symbolic link file.
// Other type of files and character device, FIFO and device file are ignored.
// For advanced copy function, see misc/unsquashfs.cpp
bool CopyFolder(const QString &src_dir, const QString &dest_dir, bool recursive = true);

// Copy file/folder mode from |src_file| to |dest_file|
// Both |src_file| and |dest_file| should not be symbolic link.
bool CopyMode(const char *src_file, const char *dest_file);

// Create parent folders and itself.
bool CreateDirs(const QString &dirpath);

// Create parent folders.
bool CreateParentDirs(const QString &filepath);

// Returns basename of |filepath|.
QString GetFileBasename(const QString &filepath);

// Returns lower cased extension name of |filepath|, might be empty.
QString GetFileExtname(const QString &filepath);

// Returns filename of |filepath|.
QString GetFileName(const QString &filepath);

// Get size of file. If file not found or has no access, returns 0.
qint64 GetFileSize(const QString &filepath);

// Read contents of file, returns an empty string if failed.
// DEPRECATED: call ReadTextFile() instead.
QString ReadFile(const QString &path);

// Read text file encoded in GB18030.
QString ReadGBKFile(const QString &path);

// Read content of file at |path|, and save its content into |content|.
// Returns true if succeeded, or false otherwise.
bool ReadRawFile(const QString &path, QByteArray &content);

// Read contents of text file, returns true if succeeded, or false otherwise.
// |content| holds the content of that file.
bool ReadTextFile(const QString &path, QString &content);

// Write content to file, returns true if succeeded, or false otherwise.
bool WriteTextFile(const QString &path, const QString &content);

} // namespace installer

#endif // INSTALLER_BASE_FILE_UTIL_H
