//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

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

class ControlCenterDBusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.ControlCenter1")

public:
    explicit ControlCenterDBusAdaptor(MainWindow *parent);
    virtual ~ControlCenterDBusAdaptor();

    inline MainWindow *parent() const;

public:
    const QRect rect() const;

public Q_SLOTS: // METHODS
    void Exit();
    void Hide();
    void Show();
    void ShowHome();
    void ShowPage(const QString &url);
    void Toggle();
    QString GetAllModule();

};

class DBusControlCenterGrandSearchService: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.ControlCenter1.GrandSearch")

public:
    explicit DBusControlCenterGrandSearchService(MainWindow *parent);
    virtual ~DBusControlCenterGrandSearchService();

    inline MainWindow *parent() const;

public Q_SLOTS: // METHODS
    QString Search(const QString json);
    bool Stop(const QString json);
    bool Action(const QString json);

private:
    QTimer *m_autoExitTimer;
};

} // namespace DCC_NAMESPACE
