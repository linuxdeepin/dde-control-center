//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "faceauthcontroller.h"
#include <dareader/reader.h>
#include <QPainterPath>
#include <QBuffer>
#include <QPainter>
#include <DGuiApplicationHelper>

#define FACEID_NUM    5
#define Faceimg_SIZE 210

DGUI_USE_NAMESPACE

namespace dccV25 {

FaceAuthController::FaceAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent)
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

    // 连接信号
    connect(m_model, &CharaMangerModel::tryStartInputFace, this, &FaceAuthController::onTryStartInputFace);
    connect(m_model, &CharaMangerModel::enrollStatusTips, this, &FaceAuthController::onEnrollStatusTips);
    connect(m_model, &CharaMangerModel::enrollInfoState, this, &FaceAuthController::onEnrollInfoState);
}

void FaceAuthController::startEnroll()
{
    QString newName;
    for (int i = 0; i < FACEID_NUM; ++i) {
        newName = tr("Faceprint") + QString("%1").arg(i + 1);
        if (!m_model->facesList().contains(newName)) {
            break;
        }
    }
    setAddStage(CharaMangerModel::Processing);
    m_enrollFaceTips = "";
    emit enrollFaceTipsChanged();
    m_faceImgContent = "";
    emit faceImgContentChanged();
    m_enrollFaceInProgress = true;
    m_worker->entollStart(m_model->faceDriverName(), m_model->faceCharaType(), newName);
}

void FaceAuthController::stopEnroll()
{
    m_enrollFaceInProgress = false;
    m_worker->stopEnroll();
    setAddStage(CharaMangerModel::StartState);
}

void FaceAuthController::rename(const QString &oldName, const QString &newName)
{
    m_worker->renameCharaItem(m_model->faceCharaType(), oldName, newName);
    m_worker->refreshUserEnrollList(m_model->faceDriverName(), m_model->faceCharaType());
}

void FaceAuthController::remove(const QString &id)
{
    m_worker->deleteCharaItem(m_model->faceCharaType(), id);
}

bool FaceAuthController::isAvailable() const
{
    return m_model->faceDriverVaild();
}

QString FaceAuthController::getTypeName() const
{
    return tr("Face");
}

QString FaceAuthController::faceImgContent() const
{
    return m_faceImgContent;
}

bool FaceAuthController::enrollFaceSuccess() const
{
    return m_enrollFaceSuccess;
}

QString FaceAuthController::enrollFaceTips() const
{
    return m_enrollFaceTips;
}

void FaceAuthController::onEnrollStatusTips(const QString &tips)
{
    m_enrollFaceTips = tips;
    emit enrollFaceTipsChanged();
}

void FaceAuthController::onEnrollInfoState(CharaMangerModel::AddInfoState state, const QString &tips)
{
    m_enrollFaceSuccess = state == CharaMangerModel::AddInfoState::Success;
    m_enrollFaceTips = m_enrollFaceSuccess ? tr("Use your face to unlock the device and make settings later") : tips;
    emit enrollFaceSuccessChanged();
    emit enrollFaceTipsChanged();
    emit enrollCompleted();
    stopEnroll();
    if (m_enrollFaceSuccess) {
        m_worker->refreshUserEnrollList(m_model->faceDriverName(), m_model->faceCharaType());
        setAddStage(CharaMangerModel::Success);
    } else {
        setAddStage(CharaMangerModel::Fail);
    }
}

void FaceAuthController::onTryStartInputFace(int fd)
{
    qDebug() << "add updateFaceImgContent to fd:" << fd;
    DA_read_frames(fd, this, updateFaceImgContent);
}

void FaceAuthController::updateFaceImgContent(void* const context, const DA_img *const img)
{
    if (!context)
        return;

    FaceAuthController *controller = static_cast<FaceAuthController *>(context);
    // Do not update after enroll completed.
    if (!controller->m_enrollFaceInProgress)
        return;

    QImage im((uchar *)img->data, img->width, img->height, QImage::Format_RGB888);

    QPixmap sourcePix = QPixmap::fromImage(im);
    int sourceSize = qMin(img->width, img->height);
    int offsetX = (img->width - sourceSize) / 2;
    int offsetY = (img->height - sourceSize) / 2;

    QPixmap croppedPix = sourcePix.copy(offsetX, offsetY, sourceSize, sourceSize)
                                   .scaled(Faceimg_SIZE, Faceimg_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 创建圆形遮罩
    QPixmap pix(Faceimg_SIZE, Faceimg_SIZE);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, Faceimg_SIZE, Faceimg_SIZE);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, croppedPix);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    QString encode = buffer.data().toBase64();
    controller->m_faceImgContent = QString("%1,%2").arg("data:image/png;base64").arg(encode);
    buffer.close();
    emit controller->faceImgContentChanged();
}

} // namespace dccV25
