//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "charamangerworker.h"
#include "operation/charamangermodel.h"
#include <dareader/reader.h>

namespace dccV25 {

class BiometricAuthController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CharaMangerModel *model MEMBER m_charaModel CONSTANT)
    Q_PROPERTY(CharaMangerModel::AddInfoState addStage READ addStage WRITE setAddStage NOTIFY addStageChanged)
    // face
    Q_PROPERTY(QString faceImgContent READ faceImgContent NOTIFY faceImgContentChanged FINAL)
    Q_PROPERTY(bool enrollFaceSuccess READ enrollFaceSuccess NOTIFY enrollFaceSuccessChanged FINAL)
    Q_PROPERTY(QString enrollFaceTips READ enrollFaceTips NOTIFY enrollFaceTipsChanged FINAL)

    // finger
    Q_PROPERTY(QString fingertipImagePath MEMBER m_fingertipImagePath NOTIFY fingerTipsChanged)
    Q_PROPERTY(QString fingerTitleTip MEMBER m_fingerTipTitle NOTIFY fingerTipsChanged)
    Q_PROPERTY(QString fingerMsgTip MEMBER m_fingerTipMessage NOTIFY fingerTipsChanged)

public:
    explicit BiometricAuthController(QObject *parent = nullptr);
    CharaMangerModel::AddInfoState addStage() const { return m_addStage; };
    void setAddStage(CharaMangerModel::AddInfoState stage);

public slots:
    // face
    void startFaceEnroll();
    void stopFaceEnroll();
    void renameFace(const QString &oldName, const QString &newName);
    void removeFace(QString faceid);
    QString faceImgContent();
    bool    enrollFaceSuccess();
    QString enrollFaceTips();

    // finger
    void requestStartFingerEnroll();
    void requestStopFingerEnroll();
    void requestRemoveFinger(const QString &id);
    void requestRenameFinger(const QString &id, const QString &newName);

    void onThumbsListChanged(const QStringList &thumbs);
    void onFingerEnrollRetry(const QString &title, const QString &msg);
    void onFingerEnrollStagePass(int pro);
    void onFingerEnrollFailed(const QString &title, const QString &msg);
    void onFingerEnrollCompleted();
    void onFingerEnrollDisconnected();

    void onFingerLiftTimerTimeout();

    // iris
    void requestStartIrisEnroll();
    void requestStopIrisEnroll();
    void requestRemoveIris(const QString &id);
    void requestRenameIris(const QString &id, const QString &newName);

signals:
    void addStageChanged();
    // face
    void faceImgContentChanged();
    void enrollFaceSuccessChanged();
    void enrollFaceTipsChanged();
    void enrollFaceCompleted();

    // finger
    void fingerTipsChanged();

private:
    static void updateFaceImgContent(void *const context, const DA_img *const img);

private:
    CharaMangerModel *m_charaModel = nullptr;
    CharaMangerWorker *m_charaWorker = nullptr;
    CharaMangerModel::AddInfoState      m_addStage = CharaMangerModel::AddInfoState::StartState;

    // face
    QString         m_faceImgContent;
    QString         m_enrollFaceTips;
    bool            m_enrollFaceSuccess;
    bool            m_enrollFaceInProgress;

    // finger
    QString         m_fingertipImagePath;
    QString         m_fingerTipTitle;
    QString         m_fingerTipMessage;
    int             m_fingerPro;
    bool            m_isStageOne;
    QTimer*         m_fingerLiftTimer;
};
}
