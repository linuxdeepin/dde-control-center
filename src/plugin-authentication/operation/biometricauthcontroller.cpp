//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "biometricauthcontroller.h"
#include "dccfactory.h"

#include <QPainterPath>
#include <QBuffer>
#include <QPainter>
#include <QtQml/qqml.h>
#include <DGuiApplicationHelper>

#define FACEID_NUM    5
#define IRISID_NUM 5
#define Faceimg_SIZE 210

DGUI_USE_NAMESPACE

namespace dccV25 {

DCC_FACTORY_CLASS(BiometricAuthController)

BiometricAuthController::BiometricAuthController(QObject *parent)
    : QObject(parent)
    , m_charaModel(new CharaMangerModel(this))
    , m_charaWorker(new CharaMangerWorker(m_charaModel, this))
    , m_fingerLiftTimer(new QTimer(this))
    , m_fingerAni(new QVariantAnimation(this))
{
    qmlRegisterType<CharaMangerModel>("org.deepin.dcc.account.biometric", 1, 0, "CharaMangerModel");
    connect(m_charaModel, &CharaMangerModel::tryStartInputFace, this, [this] (int fd){
        qDebug() << "add updateFaceImgContent to fd:" << fd;
        DA_read_frames(fd, this, updateFaceImgContent);
    });
    connect(m_charaModel, &CharaMangerModel::enrollStatusTips, this, [this] (const QString tips) {
        m_enrollFaceTips = tips;
        emit enrollFaceTipsChanged();
    });
    connect(m_charaModel, &CharaMangerModel::enrollInfoState, this, [this] (const CharaMangerModel::AddInfoState state, const QString &tips) {
        m_enrollFaceSuccess = state == CharaMangerModel::AddInfoState::Success;
        m_enrollFaceTips = m_enrollFaceSuccess ? tr("Use your face to unlock the device and make settings later"): tips;
        emit enrollFaceSuccessChanged();
        emit enrollFaceTipsChanged();
        emit enrollFaceCompleted();
        stopFaceEnroll();
        if (m_enrollFaceSuccess) {
            m_charaWorker->refreshUserEnrollList(m_charaModel->faceDriverName(), m_charaModel->faceCharaType());
            setAddStage(CharaMangerModel::Success);
        } else {
            setAddStage(CharaMangerModel::Fail);
        }
    });

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


    connect(m_charaModel, &CharaMangerModel::thumbsListChanged, this, &BiometricAuthController::onThumbsListChanged);
    onThumbsListChanged(m_charaModel->thumbsList());
    onFingerEnrollStagePass(0);
    m_fingerLiftTimer->setSingleShot(true);
    m_fingerLiftTimer->setInterval(1000);

    m_fingerAni->setDuration(1000);
    m_fingerAni->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_charaModel, &CharaMangerModel::enrollCompleted, this, &BiometricAuthController::onFingerEnrollCompleted);
    connect(m_charaModel, &CharaMangerModel::enrollStagePass, this, &BiometricAuthController::onFingerEnrollStagePass);
    connect(m_charaModel, &CharaMangerModel::enrollFailed, this, &BiometricAuthController::onFingerEnrollFailed);
    connect(m_charaModel, &CharaMangerModel::enrollDisconnected, this, &BiometricAuthController::onFingerEnrollDisconnected);
    connect(m_charaModel, &CharaMangerModel::enrollRetry, this, &BiometricAuthController::onFingerEnrollRetry);
    connect(m_fingerLiftTimer, &QTimer::timeout, this, &BiometricAuthController::onFingerLiftTimerTimeout);
    connect(m_fingerAni,&QVariantAnimation::valueChanged, this, &BiometricAuthController::onFingerAniValueChanged);

    setAddStage(CharaMangerModel::StartState);
}

void BiometricAuthController::setAddStage(CharaMangerModel::AddInfoState stage)
{
    m_addStage = stage;
    Q_EMIT addStageChanged();
}

void BiometricAuthController::renameFace(const QString &oldName, const QString &newName) {
    m_charaWorker->renameCharaItem(m_charaModel->faceCharaType(), oldName, newName);
    m_charaWorker->refreshUserEnrollList(m_charaModel->faceDriverName(), m_charaModel->faceCharaType());
}

void BiometricAuthController::removeFace(QString faceId) {
    m_charaWorker->deleteCharaItem(m_charaModel->faceCharaType(), faceId);
}

void BiometricAuthController::updateFaceImgContent(void* const context, const DA_img *const img) {
    if (!context)
        return;

    BiometricAuthController *controller = static_cast<BiometricAuthController *>(context);
    // Do not update after enroll completed.
    if (!controller->m_enrollFaceInProgress)
        return;

    QImage im((uchar *)img->data, img->width, img->height, QImage::Format_RGB888);
    QPixmap pix(Faceimg_SIZE, Faceimg_SIZE);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, Faceimg_SIZE, Faceimg_SIZE);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, Faceimg_SIZE, Faceimg_SIZE, QPixmap::fromImage(im));

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    QString encode = buffer.data().toBase64();
    controller->m_faceImgContent = QString("%1,%2").arg("data:image/png;base64").arg(encode);
    buffer.close();
    emit controller->faceImgContentChanged();
}

void BiometricAuthController::startFaceEnroll()
{
    QString newName;
    for (int i = 0; i < FACEID_NUM; ++i) {
        newName = tr("Faceprint") + QString("%1").arg(i + 1);
        if (!m_charaModel->facesList().contains(newName)) {
            break;
        }
    }
    setAddStage(CharaMangerModel::Processing);
    m_enrollFaceTips = "";
    emit enrollFaceTipsChanged();
    m_enrollFaceInProgress = true;
    m_charaWorker->entollStart(m_charaModel->faceDriverName(), m_charaModel->faceCharaType(), newName);
}

void BiometricAuthController::stopFaceEnroll() {
    m_enrollFaceInProgress = false;
    m_charaWorker->stopEnroll();
    setAddStage(CharaMangerModel::StartState);
}

QString BiometricAuthController::faceImgContent() {
    return m_faceImgContent;
}

bool BiometricAuthController::enrollFaceSuccess() {
    return m_enrollFaceSuccess;
}

QString BiometricAuthController::enrollFaceTips() {
    return m_enrollFaceTips;
}


void BiometricAuthController::requestStartFingerEnroll()
{
    QString newFingerName;
    auto thumbList = m_charaModel->thumbsList();
    for (const auto &predefineName : m_charaModel->getPredefineThumbsName()) {
        if (!thumbList.contains(predefineName)) {
            newFingerName = predefineName;
            break;
        }
    }
    setAddStage(CharaMangerModel::Processing);
    onFingerEnrollStagePass(0);
    m_charaWorker->tryEnroll(m_charaModel->userName(), newFingerName);
}

void BiometricAuthController::requestStopFingerEnroll()
{
    m_charaWorker->stopFingerEnroll(m_charaModel->userName());
    m_charaWorker->refreshFingerEnrollList(m_charaModel->userName());
}

void BiometricAuthController::requestRemoveFinger(const QString &id)
{
    m_charaWorker->deleteFingerItem(m_charaModel->userName(), id);
}

void BiometricAuthController::requestRenameFinger(const QString &id, const QString &newName)
{
    m_charaWorker->renameFingerItem(m_charaModel->userName(), id, newName);
}

void BiometricAuthController::onThumbsListChanged(const QStringList &thumbs)
{
    // TODO
}

void BiometricAuthController::onFingerEnrollRetry(const QString &title, const QString &msg)
{
    m_fingerTipTitle = title;
    m_fingerTipMessage = msg;
    Q_EMIT fingerTipsChanged();
}
void BiometricAuthController::onFingerEnrollStagePass(int pro)
{
    QString themeStr;

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

void BiometricAuthController::onFingerEnrollFailed(const QString &title, const QString &msg)
{
    m_fingerTipTitle = title;
    m_fingerTipMessage = msg;
    setAddStage(CharaMangerModel::Fail);
    Q_EMIT fingerTipsChanged();
    requestStopFingerEnroll();
}

void BiometricAuthController::onFingerEnrollCompleted()
{
    onFingerEnrollStagePass(100);
    setAddStage(CharaMangerModel::Success);
    requestStopFingerEnroll();
}

void BiometricAuthController::onFingerEnrollDisconnected()
{
    m_fingerTipTitle = tr("Scan Suspended");
    m_fingerTipMessage = tr("Scan Suspended");
    setAddStage(CharaMangerModel::Fail);
    Q_EMIT fingerTipsChanged();
    requestStopFingerEnroll();
}

void BiometricAuthController::onFingerLiftTimerTimeout()
{
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
    Q_EMIT fingerTipsChanged();
}

void BiometricAuthController::onFingerAniValueChanged(const QVariant &index)
{
    if (index == 150) {
        m_fingertipImagePath = "user_biometric_fingerprint_success";
    } else {
        m_fingertipImagePath = QString(":/icons/deepin/builtin/icons/%1/icons/finger/fingerprint_animation_%1_%2.png").arg(m_themeType).arg(index.toInt(), 5, 10, QChar('0'));
    }
    Q_EMIT fingerTipsChanged();
}

void BiometricAuthController::requestStartIrisEnroll()
{
    auto irislist = m_charaModel->irisList();
    if (irislist.size() >= IRISID_NUM) {
        return;
    }

    for (int i = 0; i < IRISID_NUM; ++i) {
        bool findNotUsedThumb = false;
        QString newName(tr("Iris") + QString("%1").arg(i + 1));

        for (int n = 0; n < IRISID_NUM && n < irislist.size(); ++n) {
            if (newName == irislist.at(n)) {
                findNotUsedThumb = true;
                break;
            }
        }

        if (!findNotUsedThumb) {
            // find
            break;
        }
    }
    setAddStage(CharaMangerModel::Processing);
    // TODO start iris enroll
}

void BiometricAuthController::requestStopIrisEnroll()
{

}

void BiometricAuthController::requestRemoveIris(const QString &id)
{

}

void BiometricAuthController::requestRenameIris(const QString &id, const QString &newName)
{

}
} // namespace dccV25

#include "biometricauthcontroller.moc"
