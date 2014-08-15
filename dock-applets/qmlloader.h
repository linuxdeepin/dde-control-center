/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef QMLLOADER_H
#define QMLLOADER_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDBusAbstractAdaptor>

class QmlLoader;
class AppletDBus;

class QmlLoader: public QObject{
    Q_OBJECT

public:
    QmlLoader(QObject* parent = 0);
    ~QmlLoader();


    QQmlEngine* engine;
    QQmlComponent* component;
    QQmlContext * rootContext;
    QObject * rootObject;

    void load(QUrl url);
    QString getAppletInfoListFromQml();
    Q_INVOKABLE QString getAppletVisibleFromConfig();
    Q_INVOKABLE void setAppletVisibleToConfig(QString info);

    //Bugfix: qt5 double screen switch case screen distory
    //if you do not patch Qt, only restart DockApplet is OK
    Q_INVOKABLE void restartDockApplet();

public slots:
    void appletInfosChangedSlot();

private:
    AppletDBus * m_dbus_proxyer;
};

class AppletDBus : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dde.dock.entry.AppletManager")
    Q_PROPERTY(QString appletInfoList READ appletInfoList NOTIFY appletInfosChanged)

public:
    AppletDBus(QmlLoader* parent);
    ~AppletDBus();

    QString appletInfoList() {
        return m_parent->getAppletInfoListFromQml();
    }
    Q_SIGNAL void appletInfosChanged();

    Q_SLOT void ShowApplet(QString id);
    Q_SLOT void HideApplet(QString id);

    Q_SLOT void ToggleApplet(QString id);

private:
    QmlLoader* m_parent;
};

#endif // QMLLOADER_H
