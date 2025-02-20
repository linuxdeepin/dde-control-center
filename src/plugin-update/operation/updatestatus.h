// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATESTATUS_H
#define UPDATESTATUS_H

#include <QObject>
#include "updatestatusdata.h"

class UpdateStatus : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString ABStatus READ ABStatus WRITE setABStatus NOTIFY ABStatusChanged)
    Q_PROPERTY(QString ABError READ ABError WRITE setABError NOTIFY ABErrorChanged)
    Q_PROPERTY(int TriggerBackingUpType READ TriggerBackingUpType WRITE setTriggerBackingUpType NOTIFY TriggerBackingUpTypeChanged)
    Q_PROPERTY(int TriggerBackupFailedType READ TriggerBackupFailedType WRITE setTriggerBackupFailedType NOTIFY TriggerBackupFailedTypeChanged)

public:
    explicit UpdateStatus(QObject *parent = nullptr);
    ~UpdateStatus();

    UpdateStatus(const UpdateStatus &other);

    QString ABStatus() const;
    QString ABError() const;
    int TriggerBackingUpType() const;
    int TriggerBackupFailedType() const;

    void setABStatus(const QString &ABStatus);
    void setABError(const QString &ABError);
    void setTriggerBackingUpType(int TriggerBackingUpType);
    void setTriggerBackupFailedType(int TriggerBackupFailedType);

    UpdateStatusData* statusData() const;

signals:
    void ABStatusChanged(const QString &ABStatus);
    void ABErrorChanged(const QString &ABError);
    void TriggerBackingUpTypeChanged(int TriggerBackingUpType);
    void TriggerBackupFailedTypeChanged(int TriggerBackupFailedType);

private:
    QString m_ABStatus;
    QString m_ABError;
    int m_TriggerBackingUpType;
    int m_TriggerBackupFailedType;
    UpdateStatusData* m_statusData;
};

#endif // UPDATESTATUS_H
