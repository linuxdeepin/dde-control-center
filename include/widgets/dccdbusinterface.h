// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCDBUSINTERFACE_H
#define DCCDBUSINTERFACE_H

#include "interface/namespace.h"

#include <dtkgui_global.h>

#include <QDBusAbstractInterface>

namespace DCC_NAMESPACE {

class DCCDBusInterfacePrivate;

class D_DECL_DEPRECATED_X("Use DDBusInteface") DCCDBusInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    explicit DCCDBusInterface(const QString &service,
                              const QString &path,
                              const QString &interface = QString(),
                              const QDBusConnection &connection = QDBusConnection::sessionBus(),
                              QObject *parent = nullptr);
    virtual ~DCCDBusInterface() override;

    bool serviceValid() const;
    QString suffix() const;
    void setSuffix(const QString &suffix);

    QVariant property(const char *propname);
    void setProperty(const char *propname, const QVariant &value);

Q_SIGNALS:
    void serviceValidChanged(const bool valid) const;

    DCC_DECLARE_PRIVATE(DCCDBusInterface)
};

} // namespace DCC_NAMESPACE

#endif // DCCDBUSINTERFACE_H
