//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "charamangermodel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

CharaMangerModel::CharaMangerModel(QObject *parent)
    : QObject (parent)
    , m_isFaceDriverVaild(false)
    , m_facesList(QStringList())
    , m_isIrisDriverVaild(false)
    , m_irisList(QStringList())
    , m_charaVaild(false)
{
    initFingerModel();
    connect(this, &CharaMangerModel::vaildFingerChanged, this, &CharaMangerModel::checkCharaVaild);
    connect(this, &CharaMangerModel::vaildFaceDriverChanged, this, &CharaMangerModel::checkCharaVaild);
    connect(this, &CharaMangerModel::vaildIrisDriverChanged, this, &CharaMangerModel::checkCharaVaild);
}

void CharaMangerModel::setFaceDriverVaild(bool isVaild)
{
    if (m_isFaceDriverVaild == isVaild)
        return;
    m_isFaceDriverVaild = isVaild;

    Q_EMIT vaildFaceDriverChanged(isVaild);
}

void CharaMangerModel::setFaceDriverName(const QString &driverName)
{
    if (m_faceDriverName == driverName)
        return;
    m_faceDriverName = driverName;
}

void CharaMangerModel::setFacesList(const QStringList &faces)
{
    if (faces == m_facesList)
        return;

    m_facesList = faces;
    Q_EMIT facesListChanged(faces);
}

void CharaMangerModel::setIrisDriverVaild(bool isVaild)
{
    if (m_isIrisDriverVaild == isVaild)
        return;
    m_isIrisDriverVaild = isVaild;

    Q_EMIT vaildIrisDriverChanged(isVaild);
}

void CharaMangerModel::setIrisDriverName(const QString &driverName)
{
    if (driverName == m_irisDriverName)
        return;

    m_irisDriverName = driverName;
}

void CharaMangerModel::setIrisList(const QStringList &iris)
{
    if (iris == m_irisList)
        return;

    m_irisList.clear();
    m_irisList = iris;
    Q_EMIT irisListChanged(iris);
}

void CharaMangerModel::setInputFaceFD(const int &fd)
{
    Q_EMIT tryStartInputFace(fd);
}

void CharaMangerModel::setInputIrisFD(CharaMangerModel::AddInfoState state)
{
    Q_EMIT tryStartInputIris(state);
}

void CharaMangerModel::initFingerModel()
{
    m_isFingerVaild = false;

    m_predefineThumbsNames = {
        tr("Fingerprint1"), tr("Fingerprint2"), tr("Fingerprint3"),
        tr("Fingerprint4"), tr("Fingerprint5"), tr("Fingerprint6"),
        tr("Fingerprint7"), tr("Fingerprint8"), tr("Fingerprint9"),
        tr("Fingerprint10")
    };
    m_progress = 0;
}

void CharaMangerModel::setFingerVaild(bool isVaild)
{
    if (m_isFingerVaild == isVaild)
        return;

    m_isFingerVaild = isVaild;

    Q_EMIT vaildFingerChanged(isVaild);
}

void CharaMangerModel::setUserName(const QString &name)
{
    if (m_userName != name)
        m_userName = name;
}

void CharaMangerModel::onFingerEnrollStatusChanged(int code, const QString& msg)
{
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;

    if (!msg.isEmpty()) {
        jsonDocument = QJsonDocument::fromJson(msg.toLocal8Bit());
        jsonObject = jsonDocument.object();
    }

    switch(code) {
    case ET_Completed:
        m_progress = 0;
        Q_EMIT enrollCompleted();
        break;
    case ET_Failed: {
        m_progress = 0;
        QString title = tr("Scan failed");
        QString msg = "";
        do {
            QStringList keys = jsonObject.keys();
            if (!keys.contains("subcode")) {
                break;
            }
            auto errCode = jsonObject.value("subcode").toInt();
            switch(errCode) {
            case FC_RepeatTemplet:
                title = tr("The fingerprint already exists");
                msg = tr("Please scan other fingers");
                break;
            case FC_UnkownError:
                title = tr("Unknown error");
                msg = tr("Scan suspended");
            }
            break;
        } while(0);
        Q_EMIT enrollFailed(title, msg);
        break;
    }
    case ET_StagePass: {
        if (msg.isEmpty()) {
            // 厂商未给出进度值的情况,直接让进度值递减增加,无限趋近于100,直到录入完成
            m_progress += (100 - m_progress)/3;
            Q_EMIT enrollStagePass(m_progress);
            break;
        }
        QStringList keys = jsonObject.keys();
        if (!keys.contains("progress")) {
            // 厂商未给出进度值的情况,直接让进度值递减增加,无限趋近于100,直到录入完成
            m_progress += (100 - m_progress)/3;
            Q_EMIT enrollStagePass(m_progress);
            break;
        }
        auto pro = jsonObject.value("progress").toInt();
        Q_EMIT enrollStagePass(pro);
        break;
    }
    case ET_Retry: {
        QString title = tr("Cannot recognize");
        QString msg = tr("Cannot recognize");
        do {
            QStringList keys = jsonObject.keys();
            if (!keys.contains("subcode")) {
                break;
            }
            auto errCode = jsonObject.value("subcode").toInt();
            switch(errCode) {
            case RC_TouchTooShort: //接触时间过短
                title = tr("Moved too fast");
                msg = tr("Finger moved too fast, please do not lift until prompted");
                break;
            case RC_ErrorFigure: //图像不可用
                title = tr("Unclear fingerprint");
                msg = tr("Clean your finger or adjust the finger position, and try again");
                break;
            case RC_RepeatTouchData: //重复率过高
                title = tr("Already scanned");
                msg = tr("Adjust the finger position to scan your fingerprint fully");
                break;
            case RC_RepeatFingerData: //重复手指
                title = tr("The fingerprint already exists");
                msg = tr("Please scan other fingers");
                break;
            case RC_SwipeTooShort: //按压时间短
                title = tr("Moved too fast");
                msg = tr("Finger moved too fast. Please do not lift until prompted");
                break;
            case RC_FingerNotCenter: //手指不在中间
                msg = tr("Adjust the finger position to scan your fingerprint fully");
                break;
            case RC_RemoveAndRetry: // 拿开手指从新扫描
                msg = tr("Clean your finger or adjust the finger position, and try again");
                break;
            case RC_CannotRecognize:
                title = tr("Cannot recognize");
                msg = tr("Lift your finger and place it on the sensor again");
                break;
            }
            break;
        } while(0);
        Q_EMIT enrollRetry(title, msg);
        break;
    }
    case ET_Disconnect:
        Q_EMIT enrollDisconnected();
        break;
    default:
        break;
    }
}

void CharaMangerModel::onTouch(const QString &id, bool pressed)
{
    Q_UNUSED(id)
    Q_UNUSED(pressed)
}

void CharaMangerModel::refreshEnrollResult(CharaMangerModel::EnrollResult enrollRes)
{
    Q_EMIT enrollResult(enrollRes);
}

void CharaMangerModel::setThumbsList(const QStringList &thumbs)
{
    if (thumbs != m_thumbsList) {
        m_thumbsList.clear();
        m_thumbsList = thumbs;
        Q_EMIT thumbsListChanged(m_thumbsList);
    }
}

void CharaMangerModel::onEnrollStatusChanged(int code, const QString &msg)
{
     // TODO： 处理所有录入状态提示信息
    Q_UNUSED(msg);
    QString title = tr("Position your face inside the frame");
    switch (code) {
    case STATUS_SUCCESS:
        Q_EMIT enrollInfoState(AddInfoState::Success, tr("Face enrolled"));
        break;
    case STATUS_CANCELED:
        break;
    case STATUS_NOT_REAL_HUMAN:
        title = tr("Position a human face please");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_NOT_CENTER:
        title = tr("Position your face inside the frame");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_TOO_BIG:
        title = tr("Keep away from the camera");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_TOO_SMALL:
        title = tr("Get closer to the camera");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_NO_FACE:
        title = tr("Position your face inside the frame");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_TOO_MANY:
        title = tr("Do not position multiple faces inside the frame");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_NOT_CLEARITY:
        title = tr("Make sure the camera lens is clean");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_BRIGHTNESS:
        title = tr("Do not enroll in dark, bright or backlit environments");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_FACE_COVERD:
        title = tr("Keep your face uncovered");
        Q_EMIT enrollStatusTips(title);
        break;
    case STATUS_OVERTIME:
        Q_EMIT enrollInfoState(AddInfoState::Fail, tr("Scan timed out"));
        break;
    case STATUS_COLLAPSE:
        Q_EMIT enrollInfoState(AddInfoState::Fail, tr("Camera occupied!"));
        break;
    default:
        break;
    }
}

void CharaMangerModel::onEnrollIrisStatusChanged(int code, const QString &msg)
{
    // TODO： 处理所有录入状态提示信息  未更新
   Q_UNUSED(msg);
   QString title = tr("Position your face inside the frame");
   switch (code) {
   case STATUS_IRIS_SUCCESS:
       Q_EMIT enrollIrisInfoState(AddInfoState::Success, tr("Face enrolled"));
       break;
   case STATUS_IRIS_TOO_BIG:
       break;
   case STATUS_IRIS_TOO_SMALL:
       title = tr("Position a human face please");
       Q_EMIT enrollIrisStatusTips(title);
       break;
   case STATUS_IRIS_NO_FACE:
       title = tr("Position your face inside the frame");
       Q_EMIT enrollIrisStatusTips(title);
       break;
   case STATUS_IRIS_NOT_CLEARITY:
       title = tr("Keep away from the camera");
       Q_EMIT enrollIrisStatusTips(title);
       break;
   case STATUS_IRIS_BRIGHTNESS:
       title = tr("Get closer to the camera");
       Q_EMIT enrollIrisStatusTips(title);
       break;
   case STATUS_IRIS_EYES_CLOSE:
       title = tr("Position your face inside the frame");
       Q_EMIT enrollIrisStatusTips(title);
       break;
   case STATUS_IRIS_CANCELED:
       Q_EMIT enrollIrisInfoState(AddInfoState::Fail, tr("Cancel"));
       break;
   case STATUS_IRIS_Error:
       Q_EMIT enrollIrisInfoState(AddInfoState::Fail, tr("Camera occupied!"));
       break;
   case STATUS_IRIS_OVERTIME:
       Q_EMIT enrollIrisInfoState(AddInfoState::Fail, tr("Scan timed out"));
       break;
   default:
       break;
   }
}

void CharaMangerModel::onRefreshEnrollDate(const int &charaType)
{
    if (charaType & FACE_CHARA) {
        qWarning()<< "=======2" << this->facesList();
        Q_EMIT facesListChanged(this->facesList());
    }

    if (charaType & IRIS_CHARA) {
        Q_EMIT irisListChanged(this->irisList());
    }
}




bool CharaMangerModel::charaVaild() const
{
    return m_charaVaild;
}

void CharaMangerModel::setCharaVaild(bool newCharaVaild)
{
    if (m_charaVaild == newCharaVaild)
        return;
    m_charaVaild = newCharaVaild;
    emit charaVaildChanged(m_charaVaild);
}
