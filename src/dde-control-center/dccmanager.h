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
    QString path() const override;

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
    QWindow *mainWindow() override;
    QAbstractItemModel *navModel() override;
    void setShowPath(const QString &path) override;
    // DBus Search
    QString search(const QString json);
    bool stop(const QString json);
    bool action(const QString json);

    // Q_SIGNALS:
    //     void pathChanged(const QString &path);
    //     void rootChanged(DccObject *root);
    //     void activeObjectChanged(DccObject *activeObject);

private:
    void initConfig();
    bool contains(const QSet<QString> &urls, const DccObject *obj);
    bool isEqual(const QString &url, const DccObject *obj);
    DccObject *findObject(const QString &url);

private Q_SLOTS:
    void updateModuleConfig(const QString &key);
    void onTriggered();
    void onVisible(bool visible);
    bool addObjectToParent(DccObject *obj);
    bool removeObjectFromParent(DccObject *obj);

private:
    DccObject *m_root;
    DccObject *m_activeObject;
    QVector<DccObject *> m_hideObjects;  // 隐藏的项
    QVector<DccObject *> m_noAddObjects; // 未找到父对象的
    QSet<DccObject *> m_noParentObjects; // 没有父对象的

    QVector<DccObject *> m_currentObjects;

    PluginManager *m_plugins;
    QWindow *m_window;
    Dtk::Core::DConfig *m_dconfig;
    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
    QQmlApplicationEngine *m_engine;
    NavigationModel *m_navModel;
};
} // namespace dccV25
#endif // DCCMANAGER_H
