// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defappmodel.h"

#include "QtQml/qqml.h"
#include "categorymodel.h"
#include "dccfactory.h"
#include "defappworker.h"
#include "defappworkerold.h"
#include "mimedbusproxyold.h"

DefAppModel::DefAppModel(QObject *parent)
    : QObject(parent)
{
    qmlRegisterType<CategoryModel>("org.deepin.dcc.defApp", 1, 0, "CategoryModel");
    for (int i = 0; i < Count; i++) {
        m_categoryModel[i] = new CategoryModel(new Category(this));
    }
    if (MimeDBusProxyOld::isRegisted()) {
        m_oldwork = new DefAppWorkerOld(this, this);
        m_isOldInterface = true;
        for (int i = 0; i < Count; i++) {
            m_categoryModel[i] = new CategoryModel(new Category(this));
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile, m_oldwork, &DefAppWorkerOld::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp, m_oldwork, &DefAppWorkerOld::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp, m_oldwork, &DefAppWorkerOld::onSetDefaultApp);
        }
        m_oldwork->active();
        m_oldwork->onGetListApps();
    } else {
        m_work = new DefAppWorker(this, this);
        for (int i = 0; i < Count; i++) {
            m_categoryModel[i] = new CategoryModel(new Category(this));
            connect(m_categoryModel[i], &CategoryModel::requestCreateFile, m_work, &DefAppWorker::onCreateFile);
            connect(m_categoryModel[i], &CategoryModel::requestDelUserApp, m_work, &DefAppWorker::onDelUserApp);
            connect(m_categoryModel[i], &CategoryModel::requestSetDefaultApp, m_work, &DefAppWorker::onSetDefaultApp);
        }
        m_work->active();
        m_work->onGetListApps();
    }
}

DefAppModel::~DefAppModel()
{
    for (int i = 0; i < Count; i++) {
        m_categoryModel[i]->deleteLater();
    }
}

DCC_FACTORY_CLASS(DefAppModel)

#include "defappmodel.moc"
