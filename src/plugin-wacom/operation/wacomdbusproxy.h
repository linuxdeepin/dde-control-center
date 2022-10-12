/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WACOMDBUSPROXY_H
#define WACOMDBUSPROXY_H

#include "interface/namespace.h"

#include <QObject>
class QDBusInterface;
class QDBusMessage;
class QDBusObjectPath;

namespace DCC_NAMESPACE {
class DCCDBusInterface;
}

class WacomDBusProxy :public QObject
{
    Q_OBJECT
public:
    explicit WacomDBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(bool Exist READ exist NOTIFY ExistChanged)
    bool exist();

    Q_PROPERTY(uint StylusPressureSensitive READ stylusPressureSensitive WRITE setStylusPressureSensitive NOTIFY StylusPressureSensitiveChanged)
    uint stylusPressureSensitive();
    void setStylusPressureSensitive(uint value);

    Q_PROPERTY(bool CursorMode READ cursorMode WRITE setCursorMode NOTIFY CursorModeChanged)
    bool cursorMode();
    void setCursorMode(bool value);

    Q_PROPERTY(uint EraserPressureSensitive READ eraserPressureSensitive WRITE setEraserPressureSensitive NOTIFY EraserPressureSensitiveChanged)
    uint eraserPressureSensitive();
    void setEraserPressureSensitive(uint value);

Q_SIGNALS:
    void ExistChanged(bool value) const;
    void StylusPressureSensitiveChanged(uint  value) const;
    void CursorModeChanged(bool  value) const;
    void EraserPressureSensitiveChanged(uint  value) const;

private:
    DCC_NAMESPACE::DCCDBusInterface *m_inputWacomInter;
};

#endif // WACOMDBUSPROXY_H
