// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatestatusdata.h"

UpdateStatusData::UpdateStatusData(QObject *parent)
    : QObject{ parent },
      m_securityUpgrade(""),
      m_systemUpgrade(""),
      m_unknowUpgrade("")
{
}

UpdateStatusData::UpdateStatusData(const UpdateStatusData &other)
    : QObject(other.parent()),
      m_securityUpgrade(other.m_securityUpgrade),
      m_systemUpgrade(other.m_systemUpgrade),
      m_unknowUpgrade(other.m_unknowUpgrade)
{
}

QString UpdateStatusData::securityUpgrade() const
{
    return m_securityUpgrade;
}

QString UpdateStatusData::systemUpgrade() const
{
    return m_systemUpgrade;
}

QString UpdateStatusData::unknowUpgrade() const
{
    return m_unknowUpgrade;
}

void UpdateStatusData::setSecurityUpgrade(const QString &securityUpgrade)
{
    if (m_securityUpgrade != securityUpgrade) {
        m_securityUpgrade = securityUpgrade;
        Q_EMIT securityUpgradeChanged(m_securityUpgrade);
    }
}

void UpdateStatusData::setSystemUpgrade(const QString &systemUpgrade)
{
    if (m_systemUpgrade != systemUpgrade) {
        m_systemUpgrade = systemUpgrade;
        Q_EMIT systemUpgradeChanged(m_systemUpgrade);
    }
}

void UpdateStatusData::setUnknowUpgrade(const QString &unknowUpgrade)
{
    if (m_unknowUpgrade != unknowUpgrade) {
        m_unknowUpgrade = unknowUpgrade;
        Q_EMIT unknowUpgradeChanged(m_unknowUpgrade);
    }
}
