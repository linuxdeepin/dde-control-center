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
QT_END_NAMESPACE

namespace dccV25 {
class PluginManager;

class DccManager : public DccApp
{
    Q_OBJECT
public:
    explicit DccManager(QObject *parent = nullptr);
    ~DccManager() override;

    void init();
    QQmlApplicationEngine *engine();
    void setMainWindow(QWindow *window);
    void loadModules(bool async, const QStringList &dirs);

    int width() const;
    int height() const;
    QString path() const;

    inline DccObject *root() const { return m_root; }

    inline DccObject *activeObject() const { return m_activeObject; }

    inline const QVector<DccObject *> &currentObjects() const { return m_currentObjects; }

    inline const QSet<QString> &hideModule() const { return m_hideModule; }

public Q_SLOTS:
    DccObject *object(const QString &name);
    void addObject(DccObject *obj);
    void removeObject(const DccObject *obj);
    void removeObject(const QString &name);
    void showPage(const QString &url);
    void showPage(DccObject *obj, const QString &cmd);
    QWindow *mainWindow();
    void setShowPath(const QString &path);
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
    DccObject *getObjectByUrl(DccObject *const root, const QString &url);

private Q_SLOTS:
    void updateModuleConfig(const QString &key);
    void onTriggered();

private:
    DccObject *m_root;
    DccObject *m_activeObject;
    QList<DccObject *> m_hideObject;
    QList<DccObject *> m_noAddObject;    // 未找到父对象的
    QList<DccObject *> m_noParentObject; // 没有父对象的

    QVector<DccObject *> m_currentObjects;

    PluginManager *m_plugins;
    QWindow *m_window;
    Dtk::Core::DConfig *m_dconfig;
    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
    QQmlApplicationEngine *m_engine;
};
} // namespace dccV25
#endif // DCCMANAGER_H
