// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYFILE_H
#define KEYFILE_H

#include <QMap>
#include <QVector>
#include <QFile>

typedef QMap<QString, QString> KeyMap;
typedef QMap<QString, KeyMap> MainKeyMap;

// 解析ini、desktop文件类
class KeyFile
{
public:
    explicit KeyFile(char separtor = ';');
    ~KeyFile();

    bool getBool(const QString &section, const QString &key, bool defaultValue = false);

    QString getStr(const QString &section, const QString &key, QString defaultValue = "");
    bool containKey(const QString &section, const QString &key);
    QStringList getStrList(const QString &section, const QString &key);

    void setKey(const QString &section, const QString &key, const QString &value);
    bool saveToFile(const QString &filePath);
    bool loadFile(const QString &filePath);
    QStringList getMainKeys();
    void removeSection(const QString &section);
    void removeKey(const QString &section, const QString &key);

private:
    MainKeyMap mainKeyMap; // section -> key : value
    QString filePath;
    QFile fp;
    bool modified;
    char listSeparator;
};

#endif // KEYFILE_H
