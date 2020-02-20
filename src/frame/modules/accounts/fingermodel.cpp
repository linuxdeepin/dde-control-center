/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "fingermodel.h"

#include <QJsonDocument>
#include <QJsonObject>

enum EnrollStatusType {
    ET_Completed = 0,
    ET_Failed,
    ET_StagePass,
    ET_Retry,
    ET_Disconnect
};

enum EnrollFailedCode {
    FC_UnkownError = 1,
    FC_RepeatTemplet,
    FC_EnrollBroken,
    FC_DataFull
};

enum EnrollRetryCode {
    RC_TouchTooShort = 1,
    RC_ErrorFigure,
    RC_RepeatTouchData,
    RC_RepeatFingerData,
    RC_SwipeTooShort,
    RC_FingerNotCenter,
    RC_RemoveAndRetry
};

using namespace dcc;
using namespace dcc::accounts;

FingerModel::FingerModel(QObject *parent) : QObject(parent)
{
    m_isVaild = false;
}

bool FingerModel::isVaild() const
{
    return m_isVaild;
}

void FingerModel::setIsVaild(bool isVaild)
{
    if (m_isVaild == isVaild)
        return;

    m_isVaild = isVaild;

    Q_EMIT vaildChanged(isVaild);
}

void FingerModel::onEnrollStatusChanged(int code, const QString& msg)
{
//    QString testJson = "{\"process\":\"50\", \"subcode\":{\"1\":\"error01\",\"2\":\"error02\"}}"; //测试代码
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;

    if(!msg.isEmpty()){
        jsonDocument = QJsonDocument::fromJson(msg.toLocal8Bit().data());
        jsonObject = jsonDocument.object();
    }

    switch(code) {
    case ET_Completed:
        Q_EMIT enrollCompleted();
        break;
    case ET_Failed: {
        QString msg = "Enroll Failed!";
        do {
            if (msg.isEmpty()) {
                break;
            }
            QStringList keys = jsonObject.keys();
            if (!keys.contains("subcode")) {
                break;
            }
            auto errCode = jsonObject.value("subcode").toObject().keys()[0].toInt();
            switch(errCode) {
            case FC_DataFull:
                msg = "数据满了，不能再录制更多指纹";
                break;
            case FC_EnrollBroken:
                msg = "录入中断，请重新录入";
                break;
            case FC_RepeatTemplet:
                msg = "指纹已存在，请使用其他手指重新录入";
                break;
            case FC_UnkownError:
                msg = "未知错误，请重新录入";
            }
            break;
        } while(0);
        Q_EMIT enrollFailed(msg);
        break;
    }
    case ET_StagePass: {
        if (msg.isEmpty()) {
            break;
        }
        QStringList keys = jsonObject.keys();
        if (!keys.contains("progress")) {
            break;
        }
        auto pro = jsonObject.value("progress").toInt();
        Q_EMIT enrollStagePass(pro);
        break;
    }
    case ET_Retry: {
        QString msg = "";
        do {
            QStringList keys = jsonObject.keys();
            if (!keys.contains("subcode")) {
                break;
            }
            auto errCode = jsonObject.value("subcode").toObject().keys()[0].toInt();
            switch(errCode) {
            case RC_ErrorFigure:
                msg = "请清洁手指或调整触摸位置，再次按压指纹识别器";
                break;
            case RC_SwipeTooShort:
                msg = "请调整手指按压区域以录入更多指纹";
                break;
            case RC_TouchTooShort:
                msg = "指纹采集间隙，请勿移动手指，直到提示您抬起";
                break;
            case RC_RemoveAndRetry:
                msg = "请清洁手指或调整触摸位置，再次按压指纹识别器";
                break;
            case RC_RepeatTouchData:
                msg = "请调整手指按压区域以录入更多指纹";
                break;
            case RC_FingerNotCenter:
                msg = "请调整手指按压区域以录入更多指纹";
                break;
            case RC_RepeatFingerData:
                msg = "请调整手指按压区域以录入更多指纹";
                break;
            }
            break;
        } while(0);
        Q_EMIT enrollRetry(msg);
        break;
    }
    case ET_Disconnect:
        Q_EMIT enrollDisconnected();
        break;
    default:
        break;
    }
}

void FingerModel::onTouch(const QString &id, bool pressed)
{

}

void FingerModel::setThumbsList(const QStringList &thumbs)
{
    m_thumbsList.clear();
    m_thumbsList << thumbs;

    Q_EMIT thumbsListChanged(m_thumbsList);
}

QStringList FingerModel::thumbsList() const
{
    return m_thumbsList;
}
