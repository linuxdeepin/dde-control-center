//SPDX-FileCopyrightText: 2025-2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractbiometriccontroller.h"

namespace dccV25 {

AbstractBiometricController::AbstractBiometricController(QObject *parent)
    : QObject(parent)
{
}

void AbstractBiometricController::setAddStage(CharaMangerModel::AddInfoState stage)
{
    if (m_addStage != stage) {
        m_addStage = stage;
        Q_EMIT addStageChanged();
    }
}

} // namespace dccV25 