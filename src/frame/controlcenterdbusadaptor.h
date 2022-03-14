/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.ControlCenter")

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

};

} // namespace DCC_NAMESPACE
