//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "irisauthcontroller.h"
#include <qlogging.h>
#include <DGuiApplicationHelper>

#define IRISID_NUM 5

DGUI_USE_NAMESPACE

namespace dccV25 {

IrisAuthController::IrisAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent)
    : AbstractBiometricController(parent)
    , m_model(model)
    , m_worker(worker)
{
    // 设置主题类型
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    if (type == DGuiApplicationHelper::LightType) {
        m_themeType = "light";
    } else if (type == DGuiApplicationHelper::DarkType) {
        m_themeType = "dark";
    }

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this](DGuiApplicationHelper::ColorType type) {
        if (type == DGuiApplicationHelper::LightType) {
            m_themeType = "light";
        } else if (type == DGuiApplicationHelper::DarkType) {
            m_themeType = "dark";
        }
    });

    // 连接信号（预留）
    connect(m_model, &CharaMangerModel::tryStartInputIris, this, &IrisAuthController::onTryStartInputIris);
    connect(m_model, &CharaMangerModel::enrollIrisStatusTips, this, &IrisAuthController::onEnrollIrisStatusTips);
    connect(m_model, &CharaMangerModel::enrollIrisInfoState, this, &IrisAuthController::onEnrollIrisInfoState);
}

void IrisAuthController::startEnroll()
{
    auto irislist = m_model->irisList();
    if (irislist.size() >= IRISID_NUM) {
        return;
    }

    QString newName;
    for (int i = 0; i < IRISID_NUM; ++i) {
        newName = tr("Iris") + QString("%1").arg(i + 1);
        if (!irislist.contains(newName)) {
            break;
        }
    }

    setAddStage(CharaMangerModel::Processing);
    m_worker->entollStart(m_model->irisDriverName(), m_model->irisCharaType(), newName);
}

void IrisAuthController::stopEnroll()
{
    m_enrollIrisInProgress = false;
    m_worker->stopEnroll();
    setAddStage(CharaMangerModel::StartState);
}

void IrisAuthController::rename(const QString &oldName, const QString &newName)
{
    m_worker->renameCharaItem(m_model->irisCharaType(), oldName, newName);
    m_worker->refreshUserEnrollList(m_model->irisDriverName(), m_model->irisCharaType());
    Q_UNUSED(oldName)
    Q_UNUSED(newName)
}

void IrisAuthController::remove(const QString &id)
{
    m_worker->deleteCharaItem(m_model->irisCharaType(), id);
    Q_UNUSED(id)
}

bool IrisAuthController::isAvailable() const
{
    return m_model->irisDriverVaild();
}

QString IrisAuthController::getTypeName() const
{
    return tr("Iris");
}

QString IrisAuthController::irisImgContent() const
{
    return m_irisImgContent;
}

bool IrisAuthController::enrollIrisSuccess() const
{
    return m_enrollIrisSuccess;
}

QString IrisAuthController::enrollIrisTips() const
{
    return m_enrollIrisTips;
}

void IrisAuthController::onEnrollIrisStatusTips(const QString &tips)
{
    m_enrollIrisTips = tips;
    emit enrollIrisTipsChanged();
}

void IrisAuthController::onEnrollIrisInfoState(CharaMangerModel::AddInfoState state, const QString &tips)
{
    m_enrollIrisSuccess = state == CharaMangerModel::AddInfoState::Success;
    m_enrollIrisTips = m_enrollIrisSuccess ? tr("Use your iris to unlock the device and make settings later") : tips;
    emit enrollIrisSuccessChanged();
    emit enrollIrisTipsChanged();
    emit enrollCompleted();
    stopEnroll();
    if (m_enrollIrisSuccess) {
        m_worker->refreshUserEnrollList(m_model->irisDriverName(), m_model->irisCharaType());
        setAddStage(CharaMangerModel::Success);
    } else {
        setAddStage(CharaMangerModel::Fail);
    }
}

void IrisAuthController::onTryStartInputIris(CharaMangerModel::AddInfoState state)
{
    Q_UNUSED(state)
    onEnrollIrisStatusTips("");
}

} // namespace dccV25 