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
    if (code == 1)
        m_testEnrollStatus = TestEnrollStatus::Default;
    if (code == 2)
        m_testEnrollStatus = TestEnrollStatus::Normal;
    if (code == 3) {
        m_testEnrollStatus = TestEnrollStatus::Exception;
        if (msg == '1')
            m_testException = TestException::error01;
        if (msg == '2')
            m_testException = TestException::error02;
        if (msg == '3')
            m_testException = TestException::error03;
        if (msg == '4')
            m_testException = TestException::error04;
        if (msg == '5')
            m_testException = TestException::error05;
    }
    Q_EMIT testEnrollStatusChange(m_testEnrollStatus, m_testException);


}

FingerModel::TestEnrollStatus FingerModel::testEnrollStatus() const
{
    return m_testEnrollStatus;
}

FingerModel::TestException FingerModel::testException() const
{
    return m_testException;
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

void FingerModel::deleteFingerItem(const QString& username, const QString& finger)
{
    for (int i(0); i != m_thumbsList.size(); ++i) {
        if (m_thumbsList[i].username == username) {
            for(auto fingerName : m_thumbsList[i].userThumbs) {
                if(fingerName == finger)
                    m_thumbsList[i].userThumbs.removeOne(fingerName);
                    Q_EMIT thumbsListChanged(m_thumbsList);
                    return;
            }
        }
    }
}
