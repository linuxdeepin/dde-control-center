/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "charamangermodel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

using namespace dcc;
using namespace dcc::authentication;

CharaMangerModel::CharaMangerModel(QObject *parent)
    : QObject (parent)
    , m_isFaceDriverVaild(false)
    , m_facesList(QStringList())
    , m_isIrisDriverVaild(false)
    , m_irisList(QStringList())
{
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
        Q_EMIT enrollInfoState(AddInfoState::Fail, tr("Device crashed, please scan again!"));
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
       Q_EMIT enrollIrisInfoState(AddInfoState::Fail, tr("Device crashed, please scan again!"));
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
        Q_EMIT facesListChanged(this->facesList());
    }

    if (charaType & IRIS_CHARA) {
        Q_EMIT irisListChanged(this->irisList());
    }
}


