// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#define MEDIAKEY 2

#include <QObject>
#include <QMap>
#include "modules/display/displaymodel.h"

static const QMap<QString, QString> DisplaykeyMap = { {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"},
    {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("},
    {"parenright", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"},
    {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"less", "<"}, {"greater", ">"}, {"question", "?"},
    {"minus", "-"}, {"equal", "="}, {"brackertleft", "["}, {"breckertright", "]"}, {"backslash", "\\"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"Up", "↑"},
    {"Left", "←"}, {"Down", "↓"}, {"Right", "→"}, {"asciitilde", "~"}, {"grave", "`"}, {"Control", "Ctrl"},
    {"Super_L", "Super"}, {"Super_R", "Super"}
};


namespace dcc {
namespace keyboard {

class ShortcutItem;
struct ShortcutInfo {
    QString accels;
    QString id;
    QString name;
    QString command;
    int type;
    ShortcutInfo *replace = nullptr;
    ShortcutItem *item = nullptr;

    bool operator==(const ShortcutInfo &info) const
    {
        return id == info.id && type == info.type;
    }

    QString toString()
    {
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
    enum InfoType {
        System,
        Custom,
        Media,
        Window,
        Workspace,
        AssistiveTools,
    };

    QList<ShortcutInfo *> systemInfo() const;
    QList<ShortcutInfo *> windowInfo() const;
    QList<ShortcutInfo *> workspaceInfo() const;
    QList<ShortcutInfo *> assistiveToolsInfo() const;
    QList<ShortcutInfo *> customInfo() const;
    QList<ShortcutInfo *> infos() const;

    void delInfo(ShortcutInfo *info);

    ShortcutInfo *currentInfo() const;
    void setCurrentInfo(ShortcutInfo *currentInfo);

    ShortcutInfo *getInfo(const QString &shortcut);
    void setSearchResult(const QString &searchResult);
    bool getWindowSwitch();
    QString parseKeystroke(QString& shortcuts);

Q_SIGNALS:
    void listChanged(QList<ShortcutInfo *>, InfoType);
    void addCustomInfo(ShortcutInfo *info);
    void shortcutChanged(ShortcutInfo *info);
    void keyEvent(bool press, const QString &shortcut);
    void searchFinished(const QList<ShortcutInfo *> searchResult);
    void windowSwitchChanged(bool value);

public Q_SLOTS:
    void onParseInfo(const QString &info);
    void onCustomInfo(const QString &json);
    void onKeyBindingChanged(const QString &value);
    void onWindowSwitchChanged(bool value);

private:
    QString m_info;
    QList<ShortcutInfo *> m_infos;
    QList<ShortcutInfo *> m_systemInfos;
    QList<ShortcutInfo *> m_windowInfos;
    QList<ShortcutInfo *> m_workspaceInfos;
    QList<ShortcutInfo *> m_assistiveToolsInfos;
    QList<ShortcutInfo *> m_customInfos;
    QList<ShortcutInfo *> m_searchList;
    ShortcutInfo *m_currentInfo = nullptr;
    bool m_windowSwitchState;
    dcc::display::DisplayModel m_dis;
};

}
}
#endif // SHORTCUTMODEL_H
