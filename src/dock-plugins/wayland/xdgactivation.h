// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <DObject>
#include <QObject>
#include <QWindow>

namespace tray {

class XdgActivationPrivate;
class XdgActivation : public QObject, public DTK_CORE_NAMESPACE::DObject
{
    Q_OBJECT
    D_DECLARE_PRIVATE(XdgActivation)
public:
    explicit XdgActivation(QObject *parent = nullptr);
    ~XdgActivation() override;

    bool isActive() const;

    void requestToken(QWindow *window = nullptr, const QString &appId = {});

Q_SIGNALS:
    void tokenReady(const QString &token);
};

} // namespace tray
