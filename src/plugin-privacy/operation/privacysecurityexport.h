// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "privacysecuritymodel.h"
#include "operation/privacysecurityworker.h"

class PrivacySecurityExport : public QObject
{
Q_OBJECT
    Q_PROPERTY(AppsModel *appsModel MEMBER m_appsModel CONSTANT)
    Q_PROPERTY(PrivacySecurityWorker *worker MEMBER m_woker CONSTANT)
public:
    explicit PrivacySecurityExport(QObject *parent = nullptr);

private:
    AppsModel *m_appsModel = nullptr;
    PrivacySecurityModel *m_privacyModel = nullptr;
    PrivacySecurityWorker *m_woker = nullptr;
};
