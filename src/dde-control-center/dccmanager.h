// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCMANAGER_H
#define DCCMANAGER_H
#include "dccapp.h"
#include "dccobject.h"

#include <DConfig>
#include <DSysInfo>

#include <QDBusContext>
#include <QDBusMessage>
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
class DccImageProvider;

class DccManager : public DccApp, protected QDBusContext
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
    int sidebarWidth() const override;
    void setSidebarWidth(int width) override;

    inline DccObject *root() const override { return m_root; }

    inline DccObject *activeObject() const override { return m_activeObject; }

    Q_INVOKABLE inline const QVector<DccObject *> &currentObjects() const { return m_currentObjects; }

    Q_INVOKABLE DccApp::UosEdition uosEdition() const;
    Q_INVOKABLE Dtk::Core::DSysInfo::ProductType productType() const;

    Q_INVOKABLE bool isTreeland() const;

    inline const QSet<QString> &hideModule() const { return m_hideModule; }

public Q_SLOTS:
    DccObject *object(const QString &name) override;
    void addObject(DccObject *obj) override;
    void removeObject(DccObject *obj) override;
    void removeObject(const QString &name) override;
    void showPage(const QString &url) override;
    void showPage(DccObject *obj);
    void showPage(DccObject *obj, const QString &cmd) override;
    void toBack();
    QWindow *mainWindow() const override;
    QAbstractItemModel *navModel() const override;
    QSortFilterProxyModel *searchModel() const override;
    void cacheImage(const QString &id, const QSize &thumbnailSize = QSize());

    void show();
    void showHelp();
    // DBus Search
    QString search(const QString &json) const;
    QString searchProxy(const QString &json) const;
    bool stop(const QString &json);
    bool action(const QString &json);
    QString GetAllModule();

Q_SIGNALS:
    void activeItemChanged(QQuickItem *item);
    void hideModuleChanged(const QSet<QString> &hideModule);

private:
    void initConfig();
    bool contains(const QSet<QString> &urls, const DccObject *obj);
    bool isMatch(const QString &url, const DccObject *obj);
    bool isEqual(const QString &url, const DccObject *obj);
    DccObject *findObject(const QString &url, bool onlyRoot = false);
    QVector<DccObject *> findObjects(const QString &url, bool onlyRoot = false, bool one = false);
    DccObject *findParent(const DccObject *obj);
    bool eventFilter(QObject *watched, QEvent *event) override;

private Q_SLOTS:
    void saveSize();
    void waitShowPage(const QString &url, const QDBusMessage message);
    void clearShowParam();
    void tryShow();
    void doShowPage(DccObject *obj, const QString &cmd);
    void updateModuleConfig(const QString &key);
    void onVisible(bool visible);
    void onObjectAdded(DccObject *obj);
    void onObjectRemoved(DccObject *obj);
    void onObjectDisplayChanged();
    bool addObjectToParent(DccObject *obj);
    bool removeObjectFromParent(DccObject *obj);
    void clearData();
    void waitLoadFinished() const;
    void doGetAllModule(const QDBusMessage message) const;

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
    DccImageProvider *m_imageProvider;
    int m_sidebarWidth;
    // DBus调用时，对应项还没加载完成，此处保存跳转参数
    QTimer *m_showTimer;
    QString m_showUrl;
    QDBusMessage m_showMessage;
};
} // namespace dccV25
#endif // DCCMANAGER_H
