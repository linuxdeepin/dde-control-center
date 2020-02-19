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

#ifndef FINGERMODEL_H
#define FINGERMODEL_H

#include <QObject>

static const QStringList thumbsLists =  { "right-little-finger","left-index-finger",
                                          "right-index-finger", "left-middle-finger" ,
                                          "right-thumb" , "left-ring-finger" ,"left-little-finger" ,
                                          "right-middle-finger" , "left-thumb" ,"right-ring-finger"};

namespace dcc {
namespace accounts {
class FingerModel : public QObject
{
    Q_OBJECT
public:
    explicit FingerModel(QObject *parent = nullptr);

    struct UserThumbs {
        QString username;
        QStringList userThumbs;

        bool operator ==(const UserThumbs &user) {
            return username == user.username && userThumbs == user.userThumbs;
        }
    };

    enum EnrollStatus{
        Ready,
        Next,
        Retry,
        Finished
    };

    enum TestEnrollStatus{
        completed,
        Default,
        StagePassed,
        enrollFinished,
        enrollFailed,
        retry
    };
    enum TestMsg{
        unknown_error,              //未知错误
        repeated_template,          //重复模板
        enroll_interrupted,         //录入中断
        data_full,                  //数据满了
        time_short,                 //接触时间太短
        graphics_unuse,             //图形不可用
        high_repetition_rate,       //两次触摸的指纹重复率过高
        thumb_repeated,             //当前指纹已存在
        swipe_too_short,            //滑动太短
        finger_not_centered,        //手指不在中间
        remove_and_retry,           //拿开手指再重新扫描
    };

    bool isVaild() const;
    void setIsVaild(bool isVaild);

    EnrollStatus enrollStatus() const;
    void setEnrollStatus(const EnrollStatus &enrollStatus);
    void setTestEnrollStatus(int code, const QString& msg);
    TestEnrollStatus testEnrollStatus() const;
    TestMsg testMsg() const;

    void addUserThumbs(const UserThumbs &thumbs);
    void cleanUserThumbs(const QString &user);
    QList<UserThumbs> thumbsList() const;
    void createTestThumbsbList();
    void setThumbsList(QString userName, QList<QString> listFingers);
    void deleteFingerItem(const QString& username, const QString& finger);

Q_SIGNALS:
    void vaildChanged(const bool isVaild);
    void enrollStatusChanged(EnrollStatus status);
    void thumbsListChanged(const QList<UserThumbs> &thumbs);
    void testEnrollStatus(const QString &id, int code, const QString &msg);
    void testEnrollTouch(const QString &id, bool pressed);
    void testEnrollStatusChange(TestEnrollStatus status, TestMsg msg, int process = 0);
    void enrollSuccessed();
    void enrollStoped();

private:
    bool m_isVaild{false};
    EnrollStatus m_enrollStatus;
    QList<UserThumbs> m_thumbsList;
    TestEnrollStatus m_testEnrollStatus{TestEnrollStatus::Default};
    TestMsg m_testMsg;
};
}
}

#endif // FINGERMODEL_H
