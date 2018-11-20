/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#define MEDIAKEY 2

#include <QObject>
#include <QMap>

static const QMap<QString, QString> DisplaykeyMap = { {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"},
                                                      {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("},
                                                      {"parenright", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"},
                                                      {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"less", "<"}, {"greater", ">"}, {"question", "?"},
                                                      {"minus", "-"}, {"equal", "="}, {"brackertleft", "["}, {"breckertright", "]"}, {"backslash", "\\"},
                                                      {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"Up", "↑"},
                                                      {"Left", "←"}, {"Down", "↓"}, {"Right", "→"}, {"asciitilde", "~"}, {"grave", "`"},{"Control", "Ctrl"},
                                                      {"Super_L", "Super"}, {"Super_R", "Super"} };


namespace dcc {
namespace keyboard{

class ShortcutItem;
struct ShortcutInfo
{
    ShortcutInfo();

    QString accels;
    QString id;
    QString name;
    QString command;
    quint32 type;
    ShortcutInfo* replace = nullptr;
    ShortcutItem* item;

    bool operator==(const ShortcutInfo &info) const
    {
        return id == info.id && type == info.type;
    }

    QString toString() {
        return name + accels + command + id + QString::number(type);
    }
};

typedef QList<ShortcutInfo> ShortcutInfoList;

class ShortcutModel : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutModel(QObject *parent = 0);
    ~ShortcutModel();
    enum InfoType{
        System,
        Custom,
        Media,
        Window,
        Workspace
    };

    QList<ShortcutInfo*> systemInfo() const;
    QList<ShortcutInfo*> windowInfo() const;
    QList<ShortcutInfo*> workspaceInfo() const;
    QList<ShortcutInfo*> customInfo() const;
    QList<ShortcutInfo*> infos() const;

    void delInfo(ShortcutInfo* info);

    ShortcutInfo *currentInfo() const;
    void setCurrentInfo(ShortcutInfo *currentInfo);

    ShortcutInfo * getInfo(const QString &shortcut);

Q_SIGNALS:
    void listChanged(QList<ShortcutInfo *>, InfoType);
    void addCustomInfo(ShortcutInfo* info);
    void shortcutChanged(ShortcutInfo *info);
    void keyEvent(bool press, const QString &shortcut);

public Q_SLOTS:
    void onParseInfo(const QString& info);
    void onCustomInfo(const QString& json);
    void onKeyBindingChanged(const QString &value);

private:
    QString m_info;
    QList<ShortcutInfo*> m_infos;
    QList<ShortcutInfo*> m_systemInfos;
    QList<ShortcutInfo*> m_windowInfos;
    QList<ShortcutInfo*> m_workspaceInfos;
    QList<ShortcutInfo*> m_customInfos;
    ShortcutInfo *m_currentInfo;
};

}
}
#endif // SHORTCUTMODEL_H
