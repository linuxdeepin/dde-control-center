//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "thememodel.h"

#include <QIcon>

ThemeViewModel::ThemeViewModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_themeModel(nullptr)
{

}

void ThemeViewModel::setThemeModel(ThemeModel *model)
{
    m_themeModel = model;
    connect(m_themeModel, &ThemeModel::defaultChanged, this, &ThemeViewModel::updateData);
    connect(m_themeModel, &ThemeModel::picAdded, this, &ThemeViewModel::updateData);
    connect(m_themeModel, &ThemeModel::itemAdded, this, &ThemeViewModel::updateData);
    connect(m_themeModel, &ThemeModel::itemRemoved, this, &ThemeViewModel::updateData);
    updateData();
}

QModelIndex ThemeViewModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_keys.size())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex ThemeViewModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ThemeViewModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_keys.size();

    return 0;
}

int ThemeViewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ThemeViewModel::data(const QModelIndex &index, int role) const
{
    if (m_keys.isEmpty() || !index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_keys.size())
        return QVariant();

    switch (role) {
    case ThemeViewModel::NameRole:
        return m_themeModel->getList().value(m_keys.at(row))["Name"].toString();
    case Qt::ToolTipRole:
        return m_themeModel->getList().value(m_keys.at(row))["Comment"].toString();
    case Qt::DecorationRole:
        return QIcon(m_themeModel->getPicList().value(m_keys.at(row)));
    case Qt::CheckStateRole: {
        QString id = m_themeModel->getDefault();
        if (id.endsWith(".light")) {
            id.chop(6);
        } else if (id.endsWith(".dark")) {
            id.chop(5);
        }
        return m_keys.at(row) == id ? Qt::Checked : Qt::Unchecked;
    }
    case ThemeViewModel::IdRole:
        return m_keys.at(row);
    case ThemeViewModel::PicRole: {
        QString pic = m_themeModel->getPicList().value(m_keys.at(row));
        return pic.startsWith("/") ? QUrl::fromLocalFile(pic).toString() : pic;
    }
    default:
        break;
    }
    return QVariant();
}

void ThemeViewModel::updateData()
{
    QStringList keys = m_themeModel->keys();
    if (keys.contains("custom")) {
        keys.removeAll("custom");
    }
    // 按字母排序（不区分大小写）
    std::sort(keys.begin(), keys.end(), [this](const QString &a, const QString &b) {
        return m_themeModel->getList().value(a)["Name"].toString().toLower() < m_themeModel->getList().value(b)["Name"].toString().toLower();
    });
    // custom主题始终放在最前面
    if (m_themeModel->keys().contains("custom")) {
        keys.push_front("custom");
    }
    beginResetModel();
    m_keys = keys;
    endResetModel();
}

QHash<int, QByteArray> ThemeViewModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[PicRole] = "pic";
    roles[Qt::CheckStateRole] = "checked";
    return roles;
}

ThemeModel::ThemeModel(QObject *parent)
    : QObject(parent)
{
}

QStringList ThemeModel::keys()
{
    return m_keys;
}

void ThemeModel::addItem(const QString &id, const QJsonObject &json)
{
    if (m_list.contains(id)) {
        m_keys.removeOne(id);
        m_keys.append(id);
        return;
    }
    m_keys.append(id);
    m_list.insert(id, json);
    Q_EMIT itemAdded(json);
}

void ThemeModel::setDefault(const QString &value)
{
    m_default = value;
    Q_EMIT defaultChanged(value);
    for (const auto &themeItem : m_list) {
        if (themeItem.value("Id") == value) {
            const QString &themeName = themeItem.value("Name").toString();
            m_currentThemeName = themeName;
            Q_EMIT currentThemeNameChanged(themeName);
            break;
        }
    }
}

QMap<QString, QString> ThemeModel::getPicList() const
{
    return m_picList;
}

void ThemeModel::addPic(const QString &id, const QString &picPath)
{
    m_picList.insert(id, picPath);
    Q_EMIT picAdded(id, picPath);
}

void ThemeModel::removeItem(const QString &id)
{
    m_list.remove(id);
    m_keys.removeOne(id);
    Q_EMIT itemRemoved(id);
}
