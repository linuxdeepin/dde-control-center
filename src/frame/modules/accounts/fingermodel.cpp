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

    m_predefineThumbsNames = {
        {"Fingerprint1", tr("Fingerprint1")},
        {"Fingerprint2", tr("Fingerprint2")},
        {"Fingerprint3", tr("Fingerprint3")},
        {"Fingerprint4", tr("Fingerprint4")},
        {"Fingerprint5", tr("Fingerprint5")},
        {"Fingerprint6", tr("Fingerprint6")},
        {"Fingerprint7", tr("Fingerprint7")},
        {"Fingerprint8", tr("Fingerprint8")},
        {"Fingerprint9", tr("Fingerprint9")},
        {"Fingerprint10", tr("Fingerprint10")}
    };
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
        QString title = "Enroll Failed!";
        QString msg = "Enroll Failed!";
        do {
            if (msg.isEmpty()) {
                break;
            }
            QStringList keys = jsonObject.keys();
            if (!keys.contains("subcode")) {
                break;
            }
            auto errCode = jsonObject.value("subcode").toInt();
            switch(errCode) {
            case FC_DataFull:
                msg = "数据满了，不能再录制更多指纹";
                break;
            case FC_EnrollBroken:
                msg = "录入中断，请重新录入";
                break;
            case FC_RepeatTemplet:
                title = tr("The fingerprint already exists");
                msg = tr("Please scan other fingers");
                break;
            case FC_UnkownError:
                title = tr("Scan failed");
                msg = tr("");
            }
            break;
        } while(0);
        Q_EMIT enrollFailed(title, msg);
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
        QString title = "Enroll Retry!";
        QString msg = "Enroll Retry!";
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
                msg = tr("Finger moved too fast. Please do not lift until prompted");
                break;
            case RC_FingerNotCenter: //手指不在中间
                msg = tr("Adjust the finger position to scan your fingerprint fully");
                break;
            case RC_RemoveAndRetry: // 拿开手指从新扫描
                msg = tr("Clean your finger or adjust the finger position, and try again");
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

void FingerModel::onTouch(const QString &id, bool pressed)
{

}

void FingerModel::setThumbsList(const QStringList &thumbs)
{
    if (thumbs != m_thumbsList) {
        m_thumbsList.clear();
        m_thumbsList << thumbs;
        Q_EMIT thumbsListChanged(m_thumbsList);
    }
}

QStringList FingerModel::thumbsList() const
{
    return m_thumbsList;
}
