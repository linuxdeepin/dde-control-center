//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QDebug>
#include <QAbstractItemModel>

class ThemeModel;
class ThemeViewModel : public QAbstractItemModel
{
public:
    enum UserDataRole {
        IdRole = Qt::UserRole + 0x101,
        NameRole,
        PicRole
    };
    explicit ThemeViewModel(QObject *parent = nullptr);
    ~ThemeViewModel() { }

    void setThemeModel(ThemeModel *model);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    void updateData();

private:
    ThemeModel *m_themeModel;
    QStringList m_keys;
};


class ThemeModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ getDefault WRITE setDefault NOTIFY defaultChanged)
    Q_PROPERTY(QString currentThemeName MEMBER m_currentThemeName NOTIFY currentThemeNameChanged)
public:
    explicit ThemeModel(QObject *parent = 0);

    QStringList keys();
    void addItem(const QString &id, const QJsonObject &json);
    QMap<QString, QJsonObject> getList() { return m_list; }

    void setDefault(const QString &value);
    inline QString getDefault() { return m_default; }

    QMap<QString, QString> getPicList() const;
    void addPic(const QString &id, const QString &picPath);

    void removeItem(const QString &id);

Q_SIGNALS:
    void itemAdded(const QJsonObject &json);
    void defaultChanged(const QString &value);
    void currentThemeNameChanged(const QString &value);
    void picAdded(const QString &id, const QString &picPath);
    void itemRemoved(const QString &id);

private:
    QMap<QString, QJsonObject> m_list;
    QString m_default;
    QMap<QString, QString> m_picList;
    QStringList m_keys;
    QString m_currentThemeName;
};

#endif // THEMEMODEL_H
