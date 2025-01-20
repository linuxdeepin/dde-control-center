// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "privacysecurityexport.h"
#include "dccfactory.h"
#include "applicationitem.h"
#include "privacysecuritymodel.h"
#include "privacysecurityworker.h"

#include <QThread>
#include <QtQml>

Q_LOGGING_CATEGORY(DCC_PRIVACY, "dde.dcc.privacy")

PrivacySecurityExport::PrivacySecurityExport(QObject *parent)
    : QObject(parent)
    , m_privacyModel(new PrivacySecurityModel(this))
    , m_woker(new PrivacySecurityWorker(m_privacyModel, this))
{
    m_appsModel = m_privacyModel->appModel();

    qmlRegisterType<ApplicationItem>("org.deepin.dcc.privacy", 1, 0, "ApplicationItem");
}

DCC_FACTORY_CLASS(PrivacySecurityExport)

#include "privacysecurityexport.moc"
