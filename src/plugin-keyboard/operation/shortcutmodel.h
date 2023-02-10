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

#include "interface/namespace.h"

#include <QObject>

namespace DCC_NAMESPACE {

class ShortcutItem;

struct ShortcutInfo
{
    QString accels;
    QString id;
    QString name;
    QString command;
    int type;
    ShortcutInfo *replace = nullptr;
    ShortcutItem *item = nullptr;

    ShortcutInfo()
        : type(0)
        , replace(nullptr)
        , item(nullptr)
    {
    }

    bool operator==(const ShortcutInfo &info) const { return id == info.id && type == info.type; }

    QString toString() { return name + accels + command + id + QString::number(type); }
};

typedef QList<ShortcutInfo> ShortcutInfoList;

class ShortcutModel : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutModel(QObject *parent = nullptr);
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
    // dcc::display::DisplayModel m_dis;
};

} // namespace DCC_NAMESPACE
#endif // SHORTCUTMODEL_H
