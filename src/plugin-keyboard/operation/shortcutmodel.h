//SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#define MEDIAKEY 2

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>

namespace dccV25 {

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
    QString sectionName;
    QString pinyin;
    int index = 0;

    ShortcutInfo()
        : type(0)
        , replace(nullptr)
        , item(nullptr)
    {
    }
    bool operator==(const ShortcutInfo &info) const
    {
        return id == info.id && type == info.type;
    }

    QString toString()
    {
        return name + accels + command + "_" + id + "_" +QString::number(type);
    }
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
    inline int count() {
        int c = m_systemInfos.count() +
                m_windowInfos.count() +
                m_workspaceInfos.count() +
                m_assistiveToolsInfos.count() +
                m_customInfos.count();
        return c;
    }
    ShortcutInfo *shortcutAt(int index, int *corners = nullptr);

    void delInfo(ShortcutInfo *info);

    ShortcutInfo *currentInfo() const;
    void setCurrentInfo(ShortcutInfo *currentInfo);

    ShortcutInfo *findInfoIf(std::function<bool (ShortcutInfo *)> cb);
    ShortcutInfo *getInfo(const QString &shortcut);

    void setSearchResult(const QString &searchResult);
    bool searchResultContains(const QString &id);
    bool getWindowSwitch();

    static QStringList formatKeys(const QString &shortcut);
Q_SIGNALS:
    void listChanged(QList<ShortcutInfo *>, InfoType);
    void addCustomInfo(ShortcutInfo *info);
    void delCustomInfo(ShortcutInfo *info);
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
    QList<ShortcutInfo *> m_windowSwitchStateInfos;
    ShortcutInfo *m_currentInfo = nullptr;
    bool m_windowSwitchState;
    //dcc::display::DisplayModel m_dis;
};

class ShortcutListModel : public QAbstractListModel {
public:
    explicit ShortcutListModel(QObject *parent = nullptr);

    enum ShortcutRole {
        SearchedTextRole = Qt::UserRole + 1,
        IdRole,
        CommandRole,
        KeySequenceRole,
        AccelsRole,
        SectionNameRole,
        CornersRole,
        IsCustomRole

    };

    void setSouceModel(ShortcutModel *model);
    ShortcutModel *souceModel();

    void reset();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    ShortcutModel *m_model = nullptr;
};

}

#endif // SHORTCUTMODEL_H
