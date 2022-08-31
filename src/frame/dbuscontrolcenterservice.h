// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DBUSCONTROLCENTER_H_1439447183
#define DBUSCONTROLCENTER_H_1439447183

#include "interface/namespace.h"

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.dde.ControlCenter
 */
namespace DCC_NAMESPACE
{
class MainWindow;
}

class DBusControlCenterService: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.ControlCenter")

public:
    explicit DBusControlCenterService(DCC_NAMESPACE::MainWindow *parent);
    virtual ~DBusControlCenterService();

    inline DCC_NAMESPACE::MainWindow *parent() const;

public: // PROPERTIES
    Q_PROPERTY(bool ShowInRight READ showInRight)
    Q_PROPERTY(QRect Rect READ rect NOTIFY rectChanged)

    bool showInRight() const;
    const QRect rect() const;

public Q_SLOTS: // METHODS
    void exitProc();
    void Hide();
    void HideImmediately();
    void Show();
    void ShowImmediately();
    void ShowHome();
    void ShowModule(const QString &module);
    void ShowPage(const QString &module, const QString &page);
    void Toggle();
    void ToggleInLeft();
    bool isNetworkCanShowPassword();
    bool isModuleAvailable(const QString &m);

Q_SIGNALS: // SIGNALS
    void rectChanged(const QRect &rect);
    void destRectChanged(const QRect &rect);

private:
    bool m_toggleProcessed;
};

class DBusControlCenterGrandSearchService: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.ControlCenter.GrandSearch")

public:
    explicit DBusControlCenterGrandSearchService(DCC_NAMESPACE::MainWindow *parent);
    virtual ~DBusControlCenterGrandSearchService();

    inline DCC_NAMESPACE::MainWindow *parent() const;

public Q_SLOTS: // METHODS
    QString Search(const QString json);
    bool Stop(const QString json);
    bool Action(const QString json);

private:
    QTimer *m_autoExitTimer;
};

#endif
