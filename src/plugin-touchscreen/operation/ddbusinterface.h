// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "dtkcore_global.h"
#include <QDBusAbstractInterface>

// DCORE_BEGIN_NAMESPACE

class DDBusInterfacePrivate;

// Imported since 5.6.3
class DDBusInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    explicit DDBusInterface(const QString &service,
                            const QString &path,
                            const QString &interface,
                            const QDBusConnection &connection = QDBusConnection::sessionBus(),
                            QObject *parent = nullptr);
    virtual ~DDBusInterface() override;

    bool serviceValid() const;
    QString suffix() const;
    void setSuffix(const QString &suffix);

    QVariant property(const char *propName);
    void setProperty(const char *propName, const QVariant &value);

Q_SIGNALS:
    void serviceValidChanged(const bool valid) const;

private:
    QScopedPointer<DDBusInterfacePrivate> d_ptr;
    Q_DECLARE_PRIVATE(DDBusInterface)
    Q_DISABLE_COPY(DDBusInterface)
};
// DCORE_END_NAMESPACE
