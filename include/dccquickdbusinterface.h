// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCQUICKDBUSINTERFACE_H
#define DCCQUICKDBUSINTERFACE_H

#include <QObject>
#include <QQmlEngine>

namespace dccV25 {
class DccQuickDBusInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString inter READ interface WRITE setInterface NOTIFY interfaceChanged)
    Q_PROPERTY(BusType connection READ connection WRITE setConnection NOTIFY connectionChanged)
public:
    explicit DccQuickDBusInterface(QObject *parent = nullptr);
    ~DccQuickDBusInterface() override;

    enum BusType {
        SessionBus,
        SystemBus,
    };
    Q_ENUM(BusType)

    QString service() const;
    void setService(const QString &service);
    QString path() const;
    void setPath(const QString &path);
    QString interface() const;
    void setInterface(const QString &interface);
    BusType connection() const;
    void setConnection(const BusType &connection);

public Q_SLOTS:
    bool callWithCallback(const QString &method, const QList<QVariant> &args, const QJSValue member, const QJSValue errorSlot);
    bool connectDbus(const QString &signature, const QJSValue slot);
    QVariant getProperty(const QString &propname);
    void setProperty(const QString &propname, const QVariant &value);

Q_SIGNALS:
    void serviceChanged(const QString &service);
    void pathChanged(const QString &path);
    void interfaceChanged(const QString &interface);
    void connectionChanged(const BusType &connection);
    // To prevent stuttering, the DBus interface uses asynchronous calls, and the correct properties can only be obtained after receiving the property Changed for the first time
    void propertyChanged(const QVariantMap &properties);

protected:
    void connectNotify(const QMetaMethod &signal) override;
    void disconnectNotify(const QMetaMethod &signal) override;

protected:
    class Private;
    Private *p_ptr;
};
} // namespace dccV25
#endif // DCCQUICKDBUSINTERFACE_H
