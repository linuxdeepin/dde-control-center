//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "charamangerworker.h"
#include "operation/charamangermodel.h"
#include "faceauthcontroller.h"
#include "fingerprintauthcontroller.h"
#include "irisauthcontroller.h"

namespace dccV25 {

class BiometricAuthController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CharaMangerModel *model MEMBER m_charaModel CONSTANT)
    
    // 直接暴露子控制器给QML使用
    Q_PROPERTY(FaceAuthController *faceController MEMBER m_faceController CONSTANT)
    Q_PROPERTY(FingerprintAuthController *fingerprintController MEMBER m_fingerprintController CONSTANT)
    Q_PROPERTY(IrisAuthController *irisController MEMBER m_irisController CONSTANT)

public:
    explicit BiometricAuthController(QObject *parent = nullptr);

private:
    CharaMangerModel *m_charaModel = nullptr;
    CharaMangerWorker *m_charaWorker = nullptr;

    // 子控制器
    FaceAuthController *m_faceController = nullptr;
    FingerprintAuthController *m_fingerprintController = nullptr;
    IrisAuthController *m_irisController = nullptr;
};
}
