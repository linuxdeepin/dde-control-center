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

FingerModel::EnrollStatus FingerModel::enrollStatus() const
{
    return m_enrollStatus;
}

void FingerModel::setEnrollStatus(const EnrollStatus &enrollStatus)
{
    m_enrollStatus = enrollStatus;

    Q_EMIT enrollStatusChanged(enrollStatus);
}

void FingerModel::setTestEnrollStatus(int code, const QString& msg)
{
    int process = 0;
//    QString testJson = "{\"process\":\"50\", \"subcode\":{\"1\":\"error01\",\"2\":\"error02\"}}"; //测试代码
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;

    if(!msg.isEmpty()){
        jsonDocument = QJsonDocument::fromJson(msg.toLocal8Bit().data());
        jsonObject = jsonDocument.object();
    }
    //成功完成，之后应该结束录入，msg为空
    if (code == 0) {
        m_testEnrollStatus = TestEnrollStatus::completed;
        Q_EMIT enrollSuccessed();
        Q_EMIT enrollStoped();
    }
    //录入失败
    if (code == 1) {
        m_testEnrollStatus = enrollFailed;
        if (!msg.isEmpty()) {
            QStringList strList = jsonObject.keys();
            for (int i = 0; i < jsonObject.size(); i++) {
                QJsonValue val = jsonObject.value(strList.at(i));
                if ("subcode" == strList.at(i) && val.isObject()) {
                    for (auto item : val.toObject().keys()) {
                        if (item == '1')
                            m_testMsg = unknown_error;
                        if (item == '2')
                            m_testMsg = repeated_template;
                        if (item == '3')
                            m_testMsg = enroll_interrupted;
                        if (item == '4')
                            m_testMsg = data_full;
                    }
                }
            }
        }
        Q_EMIT enrollStoped();
    }
    if (code == 2) {
        m_testEnrollStatus = TestEnrollStatus::StagePassed;
        QStringList strList = jsonObject.keys();
        for (int i = 0; i < jsonObject.size(); i++) {
            QJsonValue val = jsonObject.value(strList.at(i));
           if ("progress" == strList.at(i))
               process = val.toInt();
        }
    }
    //重试，之后重新录入
    if (code == 3) {
        m_testEnrollStatus = TestEnrollStatus::retry;
        //接触时间过短
        if (!msg.isEmpty()) {
            QStringList strList = jsonObject.keys();
            for (int i = 0; i < jsonObject.size(); i++) {
                QJsonValue val = jsonObject.value(strList.at(i));
                if ("subcode" == strList.at(i) && val.isObject()) {
                    for (auto item : val.toObject().keys()) {
                        if (item == '1')
                            m_testMsg = time_short;
                        if (item == '2')
                            m_testMsg = graphics_unuse;
                        if (item == '3')
                            m_testMsg = high_repetition_rate;
                        if (item == '4')
                            m_testMsg = thumb_repeated;
                        if (item == '5')
                            m_testMsg = swipe_too_short;
                        if (item == '6')
                            m_testMsg = finger_not_centered;
                        if (item == '7')
                            m_testMsg = remove_and_retry;
                    }
                }
            }
        }
    }
    Q_EMIT testEnrollStatusChange(m_testEnrollStatus, m_testMsg, process);


}

FingerModel::TestEnrollStatus FingerModel::testEnrollStatus() const
{
    return m_testEnrollStatus;
}

FingerModel::TestMsg FingerModel::testMsg() const
{
    return m_testMsg;
}

void FingerModel::addUserThumbs(const FingerModel::UserThumbs &thumbs)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList.at(i).username == thumbs.username) {
            m_thumbsList.removeAt(i);
            break;
        }
    }

    m_thumbsList << thumbs;

    Q_EMIT thumbsListChanged(m_thumbsList);
}

void FingerModel::cleanUserThumbs(const QString &user)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList[i].username == user) {
            m_thumbsList.removeAt(i);
            Q_EMIT thumbsListChanged(m_thumbsList);
            return;
        }
    }
}

QList<FingerModel::UserThumbs> FingerModel::thumbsList() const
{
    return m_thumbsList;
}

void FingerModel::createTestThumbsbList()
{
    UserThumbs userThumbs;
    userThumbs.username = "yangyuyin";
    QStringList testList;
    testList.append("A");
    testList.append("B");
    userThumbs.userThumbs = testList;
    m_thumbsList.append(userThumbs);
}

void FingerModel::setThumbsList(QString userName, QList<QString> listFingers)
{
    m_thumbsList.clear();
    UserThumbs thumb;
    thumb.username = userName;
    for (auto finger : listFingers){
        thumb.userThumbs.append(finger);
    }
    m_thumbsList.append(thumb);
    Q_EMIT thumbsListChanged(m_thumbsList);
}

void FingerModel::deleteFingerItem(const QString& username, const QString& finger)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList[i].username == username) {
            for (auto fingerName : m_thumbsList[i].userThumbs) {
                if(fingerName == finger)
                    m_thumbsList[i].userThumbs.removeOne(fingerName);
                    Q_EMIT thumbsListChanged(m_thumbsList);
                    return;
            }
        }
    }
}
