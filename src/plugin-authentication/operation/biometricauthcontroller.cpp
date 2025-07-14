//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "biometricauthcontroller.h"
#include "dccfactory.h"
#include <QtQml/qqml.h>

namespace dccV25 {

DCC_FACTORY_CLASS(BiometricAuthController)

BiometricAuthController::BiometricAuthController(QObject *parent)
    : QObject(parent)
    , m_charaModel(new CharaMangerModel(this))
    , m_charaWorker(new CharaMangerWorker(m_charaModel, this))
{
    // 注册QML类型
    qmlRegisterType<CharaMangerModel>("org.deepin.dcc.account.biometric", 1, 0, "CharaMangerModel");
    qmlRegisterType<FaceAuthController>("org.deepin.dcc.account.biometric", 1, 0, "FaceAuthController");
    qmlRegisterType<FingerprintAuthController>("org.deepin.dcc.account.biometric", 1, 0, "FingerprintAuthController");
    qmlRegisterType<IrisAuthController>("org.deepin.dcc.account.biometric", 1, 0, "IrisAuthController");
    
    // 创建子控制器
    m_faceController = new FaceAuthController(m_charaModel, m_charaWorker, this);
    m_fingerprintController = new FingerprintAuthController(m_charaModel, m_charaWorker, this);
    m_irisController = new IrisAuthController(m_charaModel, m_charaWorker, this);
}

} // namespace dccV25

#include "biometricauthcontroller.moc"
