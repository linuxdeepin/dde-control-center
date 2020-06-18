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
#include <QDebug>

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
    RC_RemoveAndRetry,
    RC_CannotRecognize
};

using namespace dcc;
using namespace dcc::accounts;

FingerModel::FingerModel(QObject *parent) : QObject(parent)
{
    m_isVaild = false;

    m_predefineThumbsNames = {
        {"Fingerprint1", tr("Fingerprint 1")},
        {"Fingerprint2", tr("Fingerprint 2")},
        {"Fingerprint3", tr("Fingerprint 3")},
        {"Fingerprint4", tr("Fingerprint 4")},
        {"Fingerprint5", tr("Fingerprint 5")},
        {"Fingerprint6", tr("Fingerprint 6")},
        {"Fingerprint7", tr("Fingerprint 7")},
        {"Fingerprint8", tr("Fingerprint 8")},
        {"Fingerprint9", tr("Fingerprint 9")},
        {"Fingerprint10", tr("Fingerprint 10")}
    };
    m_progress = 0;
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
    qDebug() << "onEnrollStatusChanged,code is: " << code << ",msg is: " << msg;
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;

    if (!msg.isEmpty()) {
        jsonDocument = QJsonDocument::fromJson(msg.toLocal8Bit().data());
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
            Q_EMIT enrollStagePass(m_progress);
            m_progress = m_progress + 20;
            if (m_progress >= 80) {
                m_progress = m_progress + 5;
            }
            break;
        }
        QStringList keys = jsonObject.keys();
        if (!keys.contains("progress")) {
            Q_EMIT enrollStagePass(m_progress);
            m_progress = m_progress + 20;
            if (m_progress >= 80) {
                m_progress = m_progress + 5;
            }
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
