//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WACOMMODELPRIVATE_P_H
#define WACOMMODELPRIVATE_P_H

#include <QObject>
#include "wacomdbusproxy.h"
#include "wacommodel.h"

class WacomModelPrivate
{
    Q_DECLARE_PUBLIC(WacomModel)
public:
    explicit WacomModelPrivate (WacomModel *object)
        : q_ptr(object)
        , m_wacomInterfaceProxy(new WacomDBusProxy(q_ptr))
    {
        initConnect();
    }

public:
    WacomModel *q_ptr;

protected:
    void existChanged(bool exist) {
        Q_Q(WacomModel);
        if (m_exist == exist) return;
        m_exist = exist;
        Q_EMIT q->ExistChanged(exist);
    };

    void cursorModeChanged(const bool cursorMode) {
        Q_Q(WacomModel);
        if (m_cursorMode == cursorMode)
            return;

        m_cursorMode = cursorMode;
        Q_EMIT q->CursorModeChanged(cursorMode);
    }

    void pressureValueChanged(const uint value) {
        Q_Q(WacomModel);
        if (m_pressureValue == value)
            return;

        m_pressureValue = value;

        Q_EMIT q->EraserPressureSensitiveChanged(value);
    }

    void initConnect() {
        QObject::connect(m_wacomInterfaceProxy, &WacomDBusProxy::ExistChanged, q_ptr, [this](bool value) -> void {
            existChanged(value);
        });

        QObject::connect(m_wacomInterfaceProxy, &WacomDBusProxy::CursorModeChanged, q_ptr, [this](bool value) ->void {
            cursorModeChanged(value);
        });

        QObject::connect(m_wacomInterfaceProxy, &WacomDBusProxy::EraserPressureSensitiveChanged, q_ptr, [this](uint value) ->void {
            pressureValueChanged(value);
        });

        existChanged(m_wacomInterfaceProxy->exist());
        cursorModeChanged(m_wacomInterfaceProxy->cursorMode());
        pressureValueChanged(m_wacomInterfaceProxy->eraserPressureSensitive());
        return;
    }

    void setCursorMode(bool value);
    void setEraserPressureSensitive(uint value);

private:
    WacomDBusProxy *m_wacomInterfaceProxy;
    bool m_exist;
    bool m_cursorMode;
    uint m_pressureValue;
};

#endif // WACOMMODELPRIVATE_P_H
