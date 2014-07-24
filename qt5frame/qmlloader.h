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

#define DBUS_NAME "com.deepin.dde.ControlCenter"
#define DBUS_PATH "/com/deepin/dde/ControlCenter"

class QmlLoader;
class QmlLoaderDBus;

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

    void toggle();
    void show();
    void showModule(QString name);
    void showImmediately();
    void hide();
    void hideImmediately();
    bool isNetworkCanShowPassword();

public slots:
    void installPackage(QString packageName);

    QString getGplText(QString language, QString type);

    void setCustomCursor(QString path);
    void clearCustomCursor();
    void setCursorFlashTime(int time);

private:
    QmlLoaderDBus * m_dbus_proxyer;
};

class QmlLoaderDBus : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DBUS_NAME)

public:
    QmlLoaderDBus(QmlLoader* parent);
    ~QmlLoaderDBus();

    Q_SLOT void Toggle();
    Q_SLOT void Show();
    Q_SLOT void ShowModule(QString name);
    Q_SLOT void ShowImmediately();
    Q_SLOT void Hide();
    Q_SLOT void HideImmediately();
    Q_SLOT bool isNetworkCanShowPassword();

private:
    QmlLoader* m_parent;
};

#endif // QMLLOADER_H
