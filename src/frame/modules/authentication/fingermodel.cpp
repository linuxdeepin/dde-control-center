// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
using namespace dcc::authentication;

FingerModel::FingerModel(QObject *parent) : QObject(parent)
{
    m_isVaild = false;

    m_predefineThumbsNames = {
        tr("Fingerprint1"), tr("Fingerprint2"), tr("Fingerprint3"),
        tr("Fingerprint4"), tr("Fingerprint5"), tr("Fingerprint6"),
        tr("Fingerprint7"), tr("Fingerprint8"), tr("Fingerprint9"),
        tr("Fingerprint10")
    };
    m_progress = 0;
}


void FingerModel::setIsVaild(bool isVaild)
{
    if (m_isVaild == isVaild)
        return;

    m_isVaild = isVaild;

    Q_EMIT vaildChanged(isVaild);
}

void FingerModel::setUserName(const QString &name)
{
    if (m_userName != name)
        m_userName = name;
}

void FingerModel::onEnrollStatusChanged(int code, const QString& msg)
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

void FingerModel::onTouch(const QString &id, bool pressed)
{
}

void FingerModel::refreshEnrollResult(FingerModel::EnrollResult enrollRes)
{
    Q_EMIT enrollResult(enrollRes);
}

void FingerModel::setThumbsList(const QStringList &thumbs)
{
    if (thumbs != m_thumbsList) {
        m_thumbsList.clear();
        m_thumbsList = thumbs;
        Q_EMIT thumbsListChanged(m_thumbsList);
    }
}

