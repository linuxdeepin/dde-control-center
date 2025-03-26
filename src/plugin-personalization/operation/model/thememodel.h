//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QDebug>

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
