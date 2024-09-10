//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WACOMDBUSPROXY_H
#define WACOMDBUSPROXY_H

#include <DDBusInterface>

#include <QObject>

class QDBusInterface;
class QDBusMessage;
class QDBusObjectPath;

using Dtk::Core::DDBusInterface;

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
    DDBusInterface *m_inputWacomInter;
};

#endif // WACOMDBUSPROXY_H
