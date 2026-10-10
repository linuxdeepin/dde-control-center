// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "xdgactivation.h"

#include <dobject_p.h>

#include <QPointer>

namespace tray {

class XdgActivationTokenV1;

class XdgActivationPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
public:
    explicit XdgActivationPrivate(XdgActivation *qq);
    ~XdgActivationPrivate() override;

    QPointer<XdgActivationTokenV1> provider;

    D_DECLARE_PUBLIC(XdgActivation)
};

} // namespace tray
