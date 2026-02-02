//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "fingerprintauthcontroller.h"
#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

namespace dccV25 {

FingerprintAuthController::FingerprintAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent)
    : AbstractBiometricController(parent)
    , m_model(model)
    , m_worker(worker)
    , m_fingerLiftTimer(new QTimer(this))
    , m_fingerAni(new QVariantAnimation(this))
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

    // 初始化指纹相关设置
    connect(m_model, &CharaMangerModel::thumbsListChanged, this, &FingerprintAuthController::onThumbsListChanged);
    onThumbsListChanged(m_model->thumbsList());
    onFingerEnrollStagePass(0);
    m_fingerLiftTimer->setSingleShot(true);
    m_fingerLiftTimer->setInterval(1000);

    m_fingerAni->setDuration(1000);
    m_fingerAni->setEasingCurve(QEasingCurve::OutCubic);

    // 连接信号
    connect(m_model, &CharaMangerModel::enrollCompleted, this, &FingerprintAuthController::onFingerEnrollCompleted);
    connect(m_model, &CharaMangerModel::enrollStagePass, this, &FingerprintAuthController::onFingerEnrollStagePass);
    connect(m_model, &CharaMangerModel::enrollFailed, this, &FingerprintAuthController::onFingerEnrollFailed);
    connect(m_model, &CharaMangerModel::enrollDisconnected, this, &FingerprintAuthController::onFingerEnrollDisconnected);
    connect(m_model, &CharaMangerModel::enrollRetry, this, &FingerprintAuthController::onFingerEnrollRetry);
    connect(m_fingerLiftTimer, &QTimer::timeout, this, &FingerprintAuthController::onFingerLiftTimerTimeout);
    connect(m_fingerAni, &QVariantAnimation::valueChanged, this, &FingerprintAuthController::onFingerAniValueChanged);
}

void FingerprintAuthController::startEnroll()
{
    // 先刷新指纹列表，确保获取到最新的设备状态
    m_worker->refreshFingerEnrollList(m_model->userName());
    
    QString newFingerName;
    auto thumbList = m_model->thumbsList();
    for (const auto &predefineName : m_model->getPredefineThumbsName()) {
        if (!thumbList.contains(predefineName)) {
            newFingerName = predefineName;
            break;
        }
    }
    setAddStage(CharaMangerModel::Processing);
    onFingerEnrollStagePass(0);
    m_worker->tryEnroll(m_model->userName(), newFingerName);
}

void FingerprintAuthController::stopEnroll()
{
    m_worker->stopFingerEnroll(m_model->userName());
    m_worker->refreshFingerEnrollList(m_model->userName());
}

void FingerprintAuthController::rename(const QString &oldName, const QString &newName)
{
    m_worker->renameFingerItem(m_model->userName(), oldName, newName);
}

void FingerprintAuthController::remove(const QString &id)
{
    m_worker->deleteFingerItem(m_model->userName(), id);
}

bool FingerprintAuthController::isAvailable() const
{
    return m_model->fingerVaild();
}

QString FingerprintAuthController::getTypeName() const
{
    return tr("Fingerprint");
}

void FingerprintAuthController::onThumbsListChanged(const QStringList &thumbs)
{
    Q_UNUSED(thumbs)
    // TODO
}

void FingerprintAuthController::onFingerEnrollRetry(const QString &title, const QString &msg)
{
    m_fingerTipTitle = title;
    m_fingerTipMessage = msg;
    emit fingerTipsChanged();
}

void FingerprintAuthController::onFingerEnrollStagePass(int pro)
{
    int startValue = m_fingerPro * 1.5;
    int endValue = pro * 1.5;

    if (m_fingerAni->state() == QVariantAnimation::Running) {
        m_fingerAni->stop();
    }
    m_fingerAni->setStartValue(startValue);
    m_fingerAni->setEndValue(endValue);
    QMetaObject::invokeMethod(m_fingerAni, "start", Qt::QueuedConnection);

    m_fingerPro = pro;
    if (m_fingerPro == 0) {
        m_isStageOne = true;
        m_fingertipImagePath = QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_animation_%1_%2.png").arg(m_themeType).arg(0, 5, 10, QChar('0'));
        m_fingerTipTitle = tr("Place your finger");
        m_fingerTipMessage = tr("Place your finger firmly on the sensor until you're asked to lift it");
    } else {
        int idx = m_fingerPro / 2;
        idx = idx > 50 ? 50 : idx;
        if (m_fingerPro > 0 && m_fingerPro < 35) {
            m_fingerTipTitle = tr("Lift your finger");
            m_fingerTipMessage = tr("Lift your finger and place it on the sensor again");
            m_fingerLiftTimer->start();
        } else if (m_fingerPro >= 35 && m_fingerPro < 100) {
            if (m_isStageOne == true) {
                m_isStageOne = false;
                m_fingerTipTitle = tr("Scan the edges of your fingerprint");
                m_fingerTipMessage = tr("Adjust the position to scan the edges of your fingerprint");
            } else {
                m_fingerTipTitle = tr("Scan the edges of your fingerprint");
                m_fingerTipMessage = tr("Lift your finger and do that again");
                m_fingerLiftTimer->start();
            }
        } else {
            m_fingerTipTitle = tr("Fingerprint added");
            m_fingerTipMessage = tr("");
        }
    }

    Q_EMIT fingerTipsChanged();
}

void FingerprintAuthController::onFingerEnrollFailed(const QString &title, const QString &msg)
{
    m_fingerTipTitle = title;
    m_fingerTipMessage = msg;
    m_fingertipImagePath = "user_biometric_fingerprint_lose";
    setAddStage(CharaMangerModel::Fail);
    emit fingerTipsChanged();
    stopEnroll();
}

void FingerprintAuthController::onFingerEnrollCompleted()
{
    onFingerEnrollStagePass(100);
    setAddStage(CharaMangerModel::Success);
    emit enrollCompleted();
    stopEnroll();
}

void FingerprintAuthController::onFingerEnrollDisconnected()
{
    m_fingerTipTitle = tr("Scan Suspended");
    m_fingerTipMessage = tr("Scan Suspended");
    setAddStage(CharaMangerModel::Fail);
    emit fingerTipsChanged();
    stopEnroll();
}

void FingerprintAuthController::onFingerLiftTimerTimeout()
{
    if (m_addStage == CharaMangerModel::Fail || m_addStage == CharaMangerModel::Success) {
        return;
    }
    QString m_defTitle;
    QString m_defTip;
    if (m_fingerPro > 0 && m_fingerPro < 35) {
        m_defTitle = tr("Place your finger");
        m_defTip = tr("Place your finger firmly on the sensor until you're asked to lift it");
    } else if (m_fingerPro >= 35 && m_fingerPro < 100) {
        m_defTitle = tr("Scan the edges of your fingerprint");
        m_defTip = tr("Place the edges of your fingerprint on the sensor");
    } else {
        m_fingerLiftTimer->stop();
        return;
    }

    m_fingerTipTitle = m_defTitle;
    m_fingerTipMessage = m_defTip;
    emit fingerTipsChanged();
}

void FingerprintAuthController::onFingerAniValueChanged(const QVariant &index)
{
    if (m_addStage == CharaMangerModel::Fail) {
        return;
    }
    if (index == 150) {
        m_fingertipImagePath = "user_biometric_fingerprint_success";
    } else {
        m_fingertipImagePath = QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_animation_%1_%2.png").arg(m_themeType).arg(index.toInt(), 5, 10, QChar('0'));
    }
    emit fingerTipsChanged();
}

} // namespace dccV25 