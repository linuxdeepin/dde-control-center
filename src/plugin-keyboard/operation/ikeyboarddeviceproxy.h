// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QtGlobal>

namespace DCC_NAMESPACE {

class IKeyboardDeviceProxy
{
public:
    virtual ~IKeyboardDeviceProxy() = default;

    virtual void active() = 0;
    virtual void deactive() {}

    virtual uint repeatDelay() const = 0;
    virtual uint repeatInterval() const = 0;
    virtual int numLockState() const = 0;

    virtual void setRepeatDelay(uint value) = 0;
    virtual void setRepeatInterval(uint value) = 0;
    virtual void setNumLockState(int value) = 0;
};

} // namespace DCC_NAMESPACE

