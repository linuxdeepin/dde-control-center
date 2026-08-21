//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "charamangerdbusproxy.h"

#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDebug>

const static QString CharaMangerService = QStringLiteral("org.deepin.dde.Authenticate1");

const static QString CharaMangerPath = QStringLiteral("/org/deepin/dde/Authenticate1/CharaManger");
const static QString CharaMangerInterface = QStringLiteral("org.deepin.dde.Authenticate1.CharaManger");

const static QString FingerprintPath = QStringLiteral("/org/deepin/dde/Authenticate1/Fingerprint");
const static QString FingerprintInterface = QStringLiteral("org.deepin.dde.Authenticate1.Fingerprint");

const static QString SessionManagerService = QStringLiteral("org.deepin.dde.SessionManager1");
const static QString SessionManagerPath = QStringLiteral("/org/deepin/dde/SessionManager1");
const static QString SessionManagerInterface = QStringLiteral("org.deepin.dde.SessionManager1");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

CharaMangerDBusProxy::CharaMangerDBusProxy(QObject *parent)
    : QObject(parent)
    , m_charaMangerInter(new QDBusInterface(CharaMangerService, CharaMangerPath, CharaMangerInterface, QDBusConnection::systemBus(), this))
    , m_fingerprintInter(new QDBusInterface(CharaMangerService, FingerprintPath, FingerprintInterface, QDBusConnection::systemBus(), this))
    , m_SMInter(new QDBusInterface(SessionManagerService, SessionManagerPath, SessionManagerInterface, QDBusConnection::sessionBus(), this))
{
    QDBusConnection::systemBus().connect(CharaMangerService, CharaMangerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect(CharaMangerService, FingerprintPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(SessionManagerService, SessionManagerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));

    connect(m_charaMangerInter, SIGNAL(EnrollStatus(const QString &, int , const QString &)),this,SIGNAL(EnrollStatusCharaManger(const QString &, int , const QString &)));
    connect(m_charaMangerInter, SIGNAL(CharaUpdated(const QString &, int)), this, SIGNAL(CharaUpdated(const QString &, int)));
    connect(m_charaMangerInter, SIGNAL(DriverChanged()), this, SIGNAL(DriverChanged()));

    connect(m_fingerprintInter, SIGNAL(EnrollStatus(const QString &, int , const QString &)),this,SIGNAL(EnrollStatusFingerprint(const QString &, int , const QString &)));
    connect(m_fingerprintInter, SIGNAL(Touch(const QString &, bool )),this,SIGNAL(Touch(const QString &, bool )));
}

void CharaMangerDBusProxy::setFingerprintInterTimeout(int timeout)
{
    m_fingerprintInter->setTimeout(timeout);
}

QString CharaMangerDBusProxy::List(const QString &driverName, int charaType)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(driverName) << QVariant::fromValue(charaType);
    return QDBusPendingReply<QString>(m_charaMangerInter->asyncCallWithArgumentList(QStringLiteral("List"), argumentList));
}

QString CharaMangerDBusProxy::driverInfo()
{
    return qvariant_cast<QString>(m_charaMangerInter->property("DriverInfo"));
}

QDBusPendingReply<QDBusUnixFileDescriptor> CharaMangerDBusProxy::EnrollStart(const QString &driverName, int charaType, const QString &charaName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(driverName) << QVariant::fromValue(charaType) << QVariant::fromValue(charaName);
    return m_charaMangerInter->asyncCallWithArgumentList(QStringLiteral("EnrollStart"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::EnrollStop()
{
    QList<QVariant> argumentList;
    return m_charaMangerInter->asyncCallWithArgumentList(QStringLiteral("EnrollStop"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::Delete(int charaType, const QString &charaName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(charaType) << QVariant::fromValue(charaName);
    return m_charaMangerInter->asyncCallWithArgumentList(QStringLiteral("Delete"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::Rename(int charaType, const QString &oldName, const QString &newName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(charaType) << QVariant::fromValue(oldName) << QVariant::fromValue(newName);
    return m_charaMangerInter->asyncCallWithArgumentList(QStringLiteral("Rename"), argumentList);
}

void CharaMangerDBusProxy::setFingerprintTimeout(int timeout)
{
    m_fingerprintInter->setTimeout(timeout);
}

QString CharaMangerDBusProxy::defaultDevice()
{
    return qvariant_cast<QString>(m_fingerprintInter->property("DefaultDevice"));
}

QDBusPendingReply<> CharaMangerDBusProxy::Claim(const QString &username, bool claimed)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(username) << QVariant::fromValue(claimed);
    return m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("Claim"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::Enroll(const QString &finger)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(finger);
    return m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("Enroll"), argumentList);

}

QStringList CharaMangerDBusProxy::ListFingers(const QString &username)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(username);
    return QDBusPendingReply<QStringList>(m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("ListFingers"), argumentList));
}

void CharaMangerDBusProxy::StopEnroll()
{
    QList<QVariant> argumentList;
    m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("StopEnroll"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::DeleteFinger(const QString &username, const QString &finger)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(username) << QVariant::fromValue(finger);
    return m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("DeleteFinger"), argumentList);
}

QDBusPendingReply<> CharaMangerDBusProxy::RenameFinger(const QString &username, const QString &finger, const QString &newName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(username) << QVariant::fromValue(finger) << QVariant::fromValue(newName);
    return m_fingerprintInter->asyncCallWithArgumentList(QStringLiteral("RenameFinger"), argumentList);
}

void CharaMangerDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
