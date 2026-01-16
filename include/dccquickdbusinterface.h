// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCQUICKDBUSINTERFACE_H
#define DCCQUICKDBUSINTERFACE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QtQml/qqml.h>

namespace dccV25 {
class DccQuickDBusInterface : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(DccDBusInterface)

    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString inter READ interface WRITE setInterface NOTIFY interfaceChanged)
    Q_PROPERTY(BusType connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
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
    // 属性前缀，防止属性与关键字冲突
    QString suffix() const;
    void setSuffix(const QString &suffix);

public Q_SLOTS:
    bool callWithCallback(const QString &method, const QList<QVariant> &args, const QJSValue member, const QJSValue errorSlot);

Q_SIGNALS:
    void serviceChanged(const QString &service);
    void pathChanged(const QString &path);
    void interfaceChanged(const QString &interface);
    void connectionChanged(const BusType &connection);
    void suffixChanged(const QString &suffix);

protected:
    void connectNotify(const QMetaMethod &signal) override;
    void disconnectNotify(const QMetaMethod &signal) override;
    void classBegin() override;
    void componentComplete() override;

protected:
    class Private;
    Private *p_ptr;
};
} // namespace dccV25
#endif // DCCQUICKDBUSINTERFACE_H
