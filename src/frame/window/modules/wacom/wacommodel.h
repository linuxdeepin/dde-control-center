/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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
