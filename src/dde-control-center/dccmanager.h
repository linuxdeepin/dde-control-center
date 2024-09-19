// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCMANAGER_H
#define DCCMANAGER_H
#include "dccapp.h"
#include "dccobject.h"

#include <DConfig>

#include <QObject>

QT_BEGIN_NAMESPACE
class QWindow;
class QQmlApplicationEngine;
class QAbstractItemModel;
QT_END_NAMESPACE

namespace dccV25 {
class NavigationModel;
class SearchModel;
class PluginManager;

class DccManager : public DccApp
{
    Q_OBJECT
public:
    explicit DccManager(QObject *parent = nullptr);
    ~DccManager() override;

    static bool installTranslator(const QString &name);
    void init();
    QQmlApplicationEngine *engine();
    void setMainWindow(QWindow *window);
    void loadModules(bool async, const QStringList &dirs);

    int width() const override;
    int height() const override;

    inline DccObject *root() const override { return m_root; }

    inline DccObject *activeObject() const override { return m_activeObject; }

    inline const QVector<DccObject *> &currentObjects() const { return m_currentObjects; }

    inline const QSet<QString> &hideModule() const { return m_hideModule; }

public Q_SLOTS:
    DccObject *object(const QString &name) override;
    void addObject(DccObject *obj) override;
    void removeObject(DccObject *obj) override;
    void removeObject(const QString &name) override;
    void showPage(const QString &url) override;
    void showPage(DccObject *obj, const QString &cmd) override;
    QWindow *mainWindow() const override;
    QAbstractItemModel *navModel() const override;
    QSortFilterProxyModel *searchModel() const override;
    void setShowPath(const QString &path) override;
    void showHelp();
    // DBus Search
    QString search(const QString &json);
    bool stop(const QString &json);
    bool action(const QString &json);

Q_SIGNALS:
    void activeItemChanged(QQuickItem *item);
    void hideModuleChanged(const QSet<QString> &hideModule);
    //     void pathChanged(const QString &path);
    //     void rootChanged(DccObject *root);
    //     void activeObjectChanged(DccObject *activeObject);

private:
    void initConfig();
    bool contains(const QSet<QString> &urls, const DccObject *obj);
    bool isEqual(const QString &url, const DccObject *obj);
    DccObject *findObject(const QString &url);

private Q_SLOTS:
    void doShowPage(DccObject *obj, const QString &cmd);
    void updateModuleConfig(const QString &key);
    void onTriggered();
    void onVisible(bool visible);
    void onObjectAdded(DccObject *obj);
    void onObjectRemoved(DccObject *obj);
    void onObjectDisplayChanged();
    bool addObjectToParent(DccObject *obj);
    bool removeObjectFromParent(DccObject *obj);
    void onQuit();

private:
    DccObject *m_root;
    DccObject *m_activeObject;    // 当前定位的项
    DccObject *m_hideObjects;     // 隐藏的项
    DccObject *m_noAddObjects;    // 未找到父对象的
    DccObject *m_noParentObjects; // 没有父对象的

    QVector<DccObject *> m_currentObjects;

    PluginManager *m_plugins;
    QWindow *m_window;
    Dtk::Core::DConfig *m_dconfig;
    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
    QQmlApplicationEngine *m_engine;
    NavigationModel *m_navModel;
    SearchModel *m_searchModel;
};
} // namespace dccV25
#endif // DCCMANAGER_H
