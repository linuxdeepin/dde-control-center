//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEJOBDBUSPROXY_H
#define UPDATEJOBDBUSPROXY_H

#include <QObject>
#include <DDBusInterface>

using Dtk::Core::DDBusInterface;

class QDBusMessage;
class QDBusInterface;
class UpdateJobDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit UpdateJobDBusProxy(const QString &jobPath, QObject *parent = nullptr);
    ~UpdateJobDBusProxy();

    bool isValid() const;
public:
    Q_PROPERTY(bool Cancelable READ cancelable NOTIFY CancelableChanged)
    bool cancelable();

    Q_PROPERTY(qlonglong CreateTime READ createTime NOTIFY CreateTimeChanged)
    qlonglong createTime();

    Q_PROPERTY(QString Description READ description NOTIFY DescriptionChanged)
    QString description();

    Q_PROPERTY(int ElapsedTime READ elapsedTime NOTIFY ElapsedTimeChanged)
    int elapsedTime();

    Q_PROPERTY(QString Id READ id NOTIFY IdChanged)
    QString id();

    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

    Q_PROPERTY(QString PackageId READ packageId NOTIFY PackageIdChanged)
    QString packageId();

    Q_PROPERTY(QStringList Packages READ packages NOTIFY PackagesChanged)
    QStringList packages();

    Q_PROPERTY(double Progress READ progress NOTIFY ProgressChanged)
    double progress();

    Q_PROPERTY(qlonglong Speed READ speed NOTIFY SpeedChanged)
    qlonglong speed();

    Q_PROPERTY(QString Status READ status NOTIFY StatusChanged)
    QString status();

    Q_PROPERTY(QString Type READ type NOTIFY TypeChanged)
    QString type();

signals:
    void Notify(int in0);
    // begin property changed signals
    void CancelableChanged(bool  value) const;
    void CreateTimeChanged(qlonglong  value) const;
    void DescriptionChanged(const QString & value) const;
    void ElapsedTimeChanged(int  value) const;
    void IdChanged(const QString & value) const;
    void NameChanged(const QString & value) const;
    void PackageIdChanged(const QString & value) const;
    void PackagesChanged(const QStringList & value) const;
    void ProgressChanged(double  value) const;
    void SpeedChanged(qlonglong  value) const;
    void StatusChanged(const QString & value) const;
    void TypeChanged(const QString & value) const;

private:
    DDBusInterface *m_updateJobInter;
};

#endif // UPDATEJOBDBUSPROXY_H
