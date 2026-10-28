// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#include "pinyinsearch.h"

#define MEDIAKEY 2

#include <QAbstractListModel>
#include <QObject>
#include <QSet>
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
    QString sectionKey;            // Stable logical category key (never translated)
    QString sectionName;           // Resolved translated display text for section
    PinyinSearchIndex pinyinIndex;
    int index = 0;

    ShortcutInfo()
        : type(0)
        , replace(nullptr)
        , item(nullptr)
    {
    }

    bool operator==(const ShortcutInfo &info) const { return id == info.id && type == info.type; }

    QString toString() { return name + accels + command + "_" + id + "_" + QString::number(type); }
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
        App, // Wayland-only (new D-Bus API)
    };

    QList<ShortcutInfo *> systemInfo() const;
    QList<ShortcutInfo *> windowInfo() const;
    QList<ShortcutInfo *> workspaceInfo() const;
    QList<ShortcutInfo *> assistiveToolsInfo() const;
    QList<ShortcutInfo *> appInfo() const;
    QList<ShortcutInfo *> customInfo() const;
    QList<ShortcutInfo *> infos() const;

    inline int count()
    {
        // Wayland dynamic grouping: rows come from m_groupInfos in groupOrder.
        if (!m_groupInfos.isEmpty()) {
            int c = 0;
            for (const auto &list : m_groupInfos)
                c += list.count();
            return c;
        }
        int c = m_systemInfos.count() + m_windowInfos.count() + m_workspaceInfos.count()
                + m_assistiveToolsInfos.count() + m_appInfos.count() + m_customInfos.count();
        return c;
    }

    ShortcutInfo *shortcutAt(int index, int *corners = nullptr);

    void delInfo(ShortcutInfo *info);

    // Wayland: remove a shortcut by id in response to a server-side
    // ShortcutRemoved (the new-API signal carries no type). Searches every
    // category list, not just custom, then reuses delInfo's delCustomInfo +
    // delete UI-refresh path. Only reached on Wayland (see shortcutRemovedById).
    void removeShortcutById(const QString &id);

    ShortcutInfo *currentInfo() const;
    void setCurrentInfo(ShortcutInfo *currentInfo);

    ShortcutInfo *findInfoIf(std::function<bool(ShortcutInfo *)> cb);
    ShortcutInfo *getInfo(const QString &shortcut);

    void setSearchResult(const QString &searchResult);
    bool searchResultContains(const QString &id);
    bool getWindowSwitch();

    // 新增：获取所有系统快捷键名称列表
    QStringList getSystemShortcutNames() const;

    // 新增：检查指定名称是否在系统快捷键中存在
    bool containsSystemShortcutName(const QString &name) const;

    static QStringList formatKeys(const QString &shortcut);
    int indexOfShortcut(ShortcutInfo *info);

    // Wayland (new API): category metadata is supplied entirely by the
    // service's ListCategories() — dcc never hardcodes category strings.
    struct CategoryMeta {
        QString key;
        QString displayName;
        int order = 0;
        bool isCustom = false;
    };
    void setCategoryMeta(const QList<CategoryMeta> &meta);
    // Display name for a section key (service metadata, then per-item
    // resolved text, then the raw key).
    QString sectionDisplayName(const QString &key) const;
    // Keys in display order, per service metadata.
    QList<QString> groupOrder() const;
    // The user-editable (Custom) category key, per service metadata.
    // On X11 (no service metadata) it is derived from the section key of a
    // type==Custom item, so the QML Custom-button check still works.
    QString customCategoryKey() const;

Q_SIGNALS:
    void listChanged(QList<ShortcutInfo *>, InfoType);
    // Wayland: category metadata (ordering/display/custom) changed — the list
    // model resets so rows re-layout in the new order.
    void categoryMetaChanged();
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
    // 清理系统快捷键名称缓存
    void invalidateSystemShortcutNamesCache() const;
    QString m_info;
    QList<ShortcutInfo *> m_infos;
    QList<ShortcutInfo *> m_systemInfos;
    QList<ShortcutInfo *> m_windowInfos;
    QList<ShortcutInfo *> m_workspaceInfos;
    QList<ShortcutInfo *> m_assistiveToolsInfos;
    QList<ShortcutInfo *> m_appInfos;
    QList<ShortcutInfo *> m_customInfos;
    // Wayland dynamic grouping: sectionKey -> items, plus service-supplied
    // category metadata (ordering / display names / custom-group identity).
    QHash<QString, QList<ShortcutInfo *>> m_groupInfos;
    QHash<QString, CategoryMeta> m_categoryMeta;
    QString m_customCategoryKey;
    QList<ShortcutInfo *> m_searchList;
    QList<ShortcutInfo *> m_windowSwitchStateInfos;
    ShortcutInfo *m_currentInfo = nullptr;
    bool m_windowSwitchState;

    // 系统快捷键名称缓存
    mutable QSet<QString> m_systemNamesCache;
    // dcc::display::DisplayModel m_dis;
};

class ShortcutListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ShortcutListModel(QObject *parent = nullptr);

    enum ShortcutRole {
        SearchedTextRole = Qt::UserRole + 1,
        IdRole,
        TypeRole,
        CommandRole,
        KeySequenceRole,
        AccelsRole,
        SectionNameRole,
        SectionKeyRole,
        CornersRole,
        IsCustomRole

    };

    void setSouceModel(ShortcutModel *model);
    ShortcutModel *souceModel();
    const ShortcutInfo *shortcutAt(int row) const;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    // 与缓存行对比后精确增删：一次 reset 会让每个分类的 DccRepeater
    // 销毁并重建全部分类与行，增删一条自定义快捷键不该付这个代价。
    void syncRows();
    void reset();
    void onUpdateShortcut(ShortcutInfo *info);

private:
    // 行缓存：同时记录结构（id/type）与数据来源指针。指针变了说明整个
    // ShortcutModel 被重新解析，行对象本身还在，只需发出 dataChanged。
    struct RowKey {
        QString id;
        int type = -1;
        const ShortcutInfo *info = nullptr;

        bool isSameRow(const RowKey &other) const
        {
            return type == other.type && id == other.id;
        }
    };

    QList<RowKey> collectRows() const;

    ShortcutModel *m_model = nullptr;
    QList<RowKey> m_rows;
};

class ShortcutFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString categoryKey READ categoryKey WRITE setCategoryKey NOTIFY categoryKeyChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit ShortcutFilterModel(QObject *parent = nullptr);

    // DccObject 化后每个分类各有一个实例：categoryKey 非空时只接受该分类的行。
    QString categoryKey() const;
    void setCategoryKey(const QString &key);

    // QML 用来判断分类是否为空（搜索过滤后隐藏空分类标题）。
    int count() const;

Q_SIGNALS:
    void categoryKeyChanged();
    void countChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    const PinyinSearchQuery &cachedPinyinQuery(const QString &pattern) const;

    QString m_categoryKey;
    mutable QString m_cachedPinyinPattern;
    mutable PinyinSearchQuery m_cachedPinyinQuery;
};

// DccObject 化后的分类列表：外层 DccRepeater 的 model。
// Wayland 按服务端 ListCategories 的顺序；X11 按固定分类列表顺序。
class ShortcutCategoryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        DisplayNameRole,
        IsCustomRole,
    };

    explicit ShortcutCategoryModel(ShortcutModel *model, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

public Q_SLOTS:
    // 分类集合（key/显示名/是否自定义）全等时不做任何事：
    // 一次 reset 会让外层 DccRepeater 重建全部分类，进而重建每张卡片里的每一行。
    void sync();

Q_SIGNALS:
    void countChanged();

private:
    struct Category {
        QString key;
        QString displayName;
        bool isCustom = false;

        bool operator==(const Category &other) const
        {
            return isCustom == other.isCustom && key == other.key
                    && displayName == other.displayName;
        }
    };

    ShortcutModel *m_model = nullptr;
    QList<Category> m_categories;
};

} // namespace dccV25

#endif // SHORTCUTMODEL_H
