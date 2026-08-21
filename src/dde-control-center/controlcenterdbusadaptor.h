// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusAbstractAdaptor>
#include <QRect>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.dde.ControlCenter
 */
#define DccDBusService "org.deepin.dde.ControlCenter1"
#define DccDBusInterface "org.deepin.dde.ControlCenter1"
#define DccDBusPath "/org/deepin/dde/ControlCenter1"

namespace dccV25 {

class DccManager;

class ControlCenterDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DccDBusInterface)

public:
    Q_PROPERTY(QRect Rect READ rect)
    Q_PROPERTY(QString Page READ page)
    Q_PROPERTY(QString Path READ path)
    explicit ControlCenterDBusAdaptor(DccManager *parent);
    virtual ~ControlCenterDBusAdaptor();

    inline DccManager *parent() const;

public:
    const QRect rect() const;
    const QString page() const;
    const QString path() const;

public Q_SLOTS: // METHODS
    void Exit();
    void Hide();
    void Show();
    void ShowHome();
    void ShowPage(const QString &url);
    void Toggle();
    QString GetAllModule();
    Q_DECL_DEPRECATED_X("Use ShowPage") void ShowPage(const QString &module, const QString &page);
    Q_DECL_DEPRECATED_X("Use ShowPage") void ShowModule(const QString &module);

private:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void updatePage();
    void updateRect();
};

class DBusControlCenterGrandSearchService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DccDBusInterface ".GrandSearch")

public:
    explicit DBusControlCenterGrandSearchService(DccManager *parent);
    virtual ~DBusControlCenterGrandSearchService();

    inline DccManager *parent() const;

public Q_SLOTS: // METHODS
    QString Search(const QString &json);
    bool Stop(const QString &json);
    bool Action(const QString &json);

private:
    QTimer *m_autoExitTimer;
    QString m_jsonCache; // 缓存下对重复请求不处理(规避全局搜索会调两次Search)
};

} // namespace dccV25
