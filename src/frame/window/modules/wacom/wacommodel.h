// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QObject>

namespace  DCC_NAMESPACE {
namespace wacom {

class WacomModel : public QObject
{
    Q_OBJECT

public:
    explicit WacomModel(QObject *parent = nullptr);
    ~WacomModel();

    bool exist() const;
    void setExist(bool exist);

    bool getCursorMode() const;
    void setCursorMode(bool cursorMode);

    void setPressureValue(const uint value);
    uint getPressureValue();

Q_SIGNALS:
    void existChanged(bool exist);
    void cursorModeChanged(const bool cursorMode);
    void pressureValueChanged(const uint value);

private:
    bool m_exist;
    bool m_cursorMode;
    uint m_pressureValue;
};
}
}
