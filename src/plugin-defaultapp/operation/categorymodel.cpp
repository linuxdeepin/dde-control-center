// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "categorymodel.h"

#include <QFileInfo>
#include <QUrl>

enum DefAppDataRole {
    DefAppIsUserRole = Qt::UserRole << (2 + 1),
    DefAppIdRole,
    DefAppCanDeleteRole,
    DefAppNameRole,
    DefAppIconRole,
    DefAppIsDefaultRole,
};

CategoryModel::CategoryModel(Category *parent)
    : QAbstractItemModel(parent)
    , m_category(parent)
{
    for (auto &&item : m_category->getappItem()) {
        onAddApp(item);
    }
    connect(m_category, &Category::addedUserItem, this, &CategoryModel::onAddApp);
    connect(m_category, &Category::removedUserItem, this, &CategoryModel::onRemoveApp);
    connect(m_category, &Category::defaultChanged, this, &CategoryModel::onDefaultChanged);
    connect(m_category, &Category::clearAll, this, &CategoryModel::resetApp);
}

CategoryModel::~CategoryModel() { }

QHash<int, QByteArray> CategoryModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names[DefAppIsUserRole] = "isUser";
    names[DefAppIdRole] = "id";
    names[DefAppCanDeleteRole] = "canDelete";
    names[DefAppNameRole] = "name";
    names[DefAppIconRole] = "icon";
    names[DefAppIsDefaultRole] = "isDefault";
    return names;
}

QModelIndex CategoryModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= m_applist.size()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex CategoryModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int CategoryModel::rowCount(const QModelIndex &) const
{
    return m_applist.size();
}

int CategoryModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_applist.size()) {
        return QModelIndex();
    }
    const App &app = m_applist.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return app.DisplayName;
    case DefAppIsUserRole:
        return app.isUser;
    case DefAppIdRole:
        return app.Id;
    case DefAppCanDeleteRole:
        return app.CanDelete;
    case DefAppNameRole:
        return app.Name;
    case DefAppIconRole:
        return app.Icon.trimmed().isEmpty() ? "application-default-icon" : app.Icon;
    case DefAppIsDefaultRole:
        return app.Id == m_category->getDefault().Id;
    default:
        break;
    }
    return QVariant();
}

void CategoryModel::addApp(const QString &path)
{
    if (path.isEmpty())
        return;

    QUrl url(path);
    QFileInfo info(url.toLocalFile());
    Q_EMIT requestCreateFile(m_category->getName(), info);
}

void CategoryModel::removeApp(const QString &id)
{
    const App *app = getAppById(id);
    if (!app || !isValid(*app)) {
        return;
    }
    Q_EMIT requestDelUserApp(m_category->getName(), *app);
}

void CategoryModel::setDefaultApp(const QString &id)
{
    const App *app = getAppById(id);
    if (!app || !isValid(*app)) {
        return;
    }
    Q_EMIT requestSetDefaultApp(m_category->getName(), *app);
}

void CategoryModel::onAddApp(const App &app)
{
    const QList<App> &applist = m_category->getappItem();
    if (!applist.contains(app))
        return;

    int index = 0;
    for (auto &&item : m_applist) {
        if ((item.CanDelete && item.CanDelete != app.CanDelete) || item.Name > app.Name) {
            break;
        }
        index++;
    }
    beginInsertRows(QModelIndex(), index, index);
    m_applist.insert(index, app);
    endInsertRows();
}

void CategoryModel::onRemoveApp(const App &app)
{
    int index = m_applist.indexOf(app);
    if (index < 0)
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_applist.remove(index);
    endRemoveRows();
}

void CategoryModel::onDefaultChanged(const App &)
{
    Q_EMIT dataChanged(createIndex(0, 0), createIndex(m_applist.size() - 1, 0), { DefAppIsDefaultRole, Qt::CheckStateRole });
}

void CategoryModel::resetApp()
{
    beginResetModel();
    m_applist = m_category->getappItem();
    endResetModel();
}

const App *CategoryModel::getAppById(const QString &appId)
{
    auto res = std::find_if(m_category->getappItem().begin(), m_category->getappItem().end(), [&appId](const App &item) -> bool {
        return item.Id == appId;
    });

    if (res != m_category->getappItem().cend()) {
        return &(*res);
    }

    return nullptr;
}
