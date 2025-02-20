// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATESTATUSDATA_H
#define UPDATESTATUSDATA_H

#include <QObject>

class UpdateStatusData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString securityUpgrade READ securityUpgrade WRITE setSecurityUpgrade NOTIFY securityUpgradeChanged)
    Q_PROPERTY(QString systemUpgrade READ systemUpgrade WRITE setSystemUpgrade NOTIFY systemUpgradeChanged)
    Q_PROPERTY(QString unknowUpgrade READ unknowUpgrade WRITE setUnknowUpgrade NOTIFY unknowUpgradeChanged)

public:
    explicit UpdateStatusData(QObject *parent = nullptr);
    UpdateStatusData(const UpdateStatusData &other);

    QString securityUpgrade() const;
    QString systemUpgrade() const;
    QString unknowUpgrade() const;

    void setSecurityUpgrade(const QString &securityUpgrade);
    void setSystemUpgrade(const QString &systemUpgrade);
    void setUnknowUpgrade(const QString &unknowUpgrade);

    bool operator!=(const UpdateStatusData &other) const {
        return m_securityUpgrade != other.m_securityUpgrade ||
               m_systemUpgrade != other.m_systemUpgrade ||
               m_unknowUpgrade != other.m_unknowUpgrade;
    }

signals:
    void securityUpgradeChanged(const QString &securityUpgrade);
    void systemUpgradeChanged(const QString &systemUpgrade);
    void unknowUpgradeChanged(const QString &unknowUpgrade);

private:
    QString m_securityUpgrade;
    QString m_systemUpgrade;
    QString m_unknowUpgrade;
   
};

#endif // UPDATESTATUSDATA_H
