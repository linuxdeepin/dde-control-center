// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GSETTINGWATCHER_H
#define GSETTINGWATCHER_H

#include <QObject>
#include <QHash>
#include <QMap>

class QGSettings;
class QListView;
class QStandardItem;
class GSettingWatcher : public QObject
{
    Q_OBJECT
public:
    static GSettingWatcher *instance();

    void bind(const QString &gsettingsName, QWidget *binder);
    void bind(const QString &gsettingsName, QListView *viewer, QStandardItem *item);
    void erase(const QString &gsettingsName);
    void erase(const QString &gsettingsName, QWidget *binder);
    void insertState(const QString &);
    const QString getStatus(const QString &gsettingsName);
    QMap<QString, bool> getMenuState();
    QVariant get(const QString &key) const;

private:
    explicit GSettingWatcher(QObject *parent = nullptr);

    void setStatus(const QString &gsettingsName, QWidget *binder);
    void setStatus(const QString &gsettingsName, QListView *viewer, QStandardItem *item);
    void onStatusModeChanged(const QString &key);
    bool existKey(const QString &key);

Q_SIGNALS:
    void requestUpdateSecondMenu(int, const QString &gsettingsName = QString());
    void requestUpdateSearchMenu(const QString &, bool);
    void requestShowSecondMenu(int); //显示第二级页面
    void notifyGSettingsChanged(const QString &, const QString &);

private:
    QMultiHash<QString, QWidget *> m_map;
    QGSettings *m_gsettings;
    QHash<QString, QPair<QListView *, QStandardItem *>> m_menuMap;
    QMap<QString, bool> m_menuState;
    QStringList m_keys;
};

#endif // GSETTINGWATCHER_H
