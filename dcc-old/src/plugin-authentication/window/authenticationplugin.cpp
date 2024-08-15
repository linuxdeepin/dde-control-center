//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "authenticationplugin.h"
#include "interface/pagemodule.h"

#include "charamangermodel.h"
#include "charamangerworker.h"
#include "faceiddetailwidget.h"
#include "fingerdetailwidget.h"
#include "irisdetailwidget.h"
#include <DIconTheme>

DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;
QString AuthenticationPlugin::name() const
{
    return QStringLiteral("Authentication");
}

ModuleObject *AuthenticationPlugin::module()
{
    // 一级
    AuthenticationModule *authenticationInterface = new AuthenticationModule;

    // 二级 -- 指纹
    ModuleObject *moduleFinger = new PageModule("fingerprint", tr("Fingerprint"), authenticationInterface);
    FingerModule *fingerPage = new FingerModule(authenticationInterface->model(), authenticationInterface->work());
    moduleFinger->appendChild(fingerPage);
    authenticationInterface->appendChild(moduleFinger);
    moduleFinger->setHidden(!authenticationInterface->model()->fingerVaild());
    connect(authenticationInterface->model(), &CharaMangerModel::vaildFingerChanged, moduleFinger, [ moduleFinger ](const bool isVaild) {
        moduleFinger->setHidden(!isVaild);
    });

    // 二级 -- 人脸
    ModuleObject *moduleFace = new PageModule("face", tr("Face"), authenticationInterface);
    FaceModule *facePage = new FaceModule(authenticationInterface->model(), authenticationInterface->work());
    moduleFace->appendChild(facePage);
    authenticationInterface->appendChild(moduleFace);
    moduleFace->setHidden(!authenticationInterface->model()->faceDriverVaild());
    connect(authenticationInterface->model(), &CharaMangerModel::vaildFaceDriverChanged, moduleFace, [ moduleFace ](const bool isVaild) {
        moduleFace->setHidden(!isVaild);
    });

    // 二级 -- 虹膜
    ModuleObject *moduleIris= new PageModule("iris", tr("Iris"), authenticationInterface);
    IrisModule *irisPage = new IrisModule(authenticationInterface->model(), authenticationInterface->work());
    moduleIris->appendChild(irisPage);
    authenticationInterface->appendChild(moduleIris);
    moduleIris->setHidden(!authenticationInterface->model()->irisDriverVaild());
    connect(authenticationInterface->model(), &CharaMangerModel::vaildIrisDriverChanged, moduleIris, [ moduleIris ](const bool isVaild) {
        moduleIris->setHidden(!isVaild);
    });

    return authenticationInterface;
}

QString AuthenticationPlugin::location() const
{
    return "3";
}

AuthenticationModule::AuthenticationModule(QObject *parent)
    : HListModule("authentication", tr("Biometric Authentication"), QString(), DIconTheme::findQIcon("dcc_nav_authentication"), parent)
    , m_model(new CharaMangerModel(this))
    , m_work(new CharaMangerWorker(m_model, this))
{
    setHidden(!m_model->charaVaild());
    connect(m_model, &CharaMangerModel::charaVaildChanged, this, [ = ](const bool isVaild) {
        setHidden(!isVaild);
    });
}

AuthenticationModule::~AuthenticationModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void AuthenticationModule::active()
{
}

QWidget *FingerModule::page()
{
    FingerDetailWidget *w = new FingerDetailWidget;
    w->setFingerModel(m_model);
    connect(w, &FingerDetailWidget::requestAddThumbs, m_worker, &CharaMangerWorker::tryEnroll);
    connect(w, &FingerDetailWidget::requestStopEnroll, m_worker, &CharaMangerWorker::stopFingerEnroll);
    connect(w, &FingerDetailWidget::requestDeleteFingerItem, m_worker, &CharaMangerWorker::deleteFingerItem);
    connect(w, &FingerDetailWidget::requestRenameFingerItem, m_worker, &CharaMangerWorker::renameFingerItem);
    connect(w, &FingerDetailWidget::noticeEnrollCompleted, m_worker, &CharaMangerWorker::refreshFingerEnrollList);
    return w;
}

QWidget *FaceModule::page()
{
    FaceidDetailWidget *w = new FaceidDetailWidget(m_model);
    connect(w, &FaceidDetailWidget::requestEntollStart, m_worker, &CharaMangerWorker::entollStart);
    connect(w, &FaceidDetailWidget::requestStopEnroll, m_worker, &CharaMangerWorker::stopEnroll);
    connect(w, &FaceidDetailWidget::requestDeleteFaceItem, m_worker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &FaceidDetailWidget::requestRenameFaceItem, m_worker, &CharaMangerWorker::renameCharaItem);
    connect(w, &FaceidDetailWidget::noticeEnrollCompleted, m_worker, &CharaMangerWorker::refreshUserEnrollList);
    return w;
}

QWidget *IrisModule::page()
{
    IrisDetailWidget *w = new IrisDetailWidget(m_model);
    connect(w, &IrisDetailWidget::requestEntollStart, m_worker, &CharaMangerWorker::entollStart);
    connect(w, &IrisDetailWidget::requestStopEnroll, m_worker, &CharaMangerWorker::stopEnroll);
    connect(w, &IrisDetailWidget::requestDeleteIrisItem, m_worker, &CharaMangerWorker::deleteCharaItem);
    connect(w, &IrisDetailWidget::requestRenameIrisItem, m_worker, &CharaMangerWorker::renameCharaItem);
    connect(w, &IrisDetailWidget::noticeEnrollCompleted, m_worker, &CharaMangerWorker::refreshUserEnrollList);
    return w;
}
