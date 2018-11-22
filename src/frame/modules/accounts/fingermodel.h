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

    bool isVaild() const;
    void setIsVaild(bool isVaild);

    EnrollStatus enrollStatus() const;
    void setEnrollStatus(const EnrollStatus &enrollStatus);

    void addUserThumbs(const UserThumbs &thumbs);
    void cleanUserThumbs(const QString &user);
    QList<UserThumbs> thumbsList() const;

Q_SIGNALS:
    void vaildChanged(const bool isVaild);
    void enrollStatusChanged(EnrollStatus status);
    void thumbsListChanged(const QList<UserThumbs> &thumbs);

private:
    bool m_isVaild;
    EnrollStatus m_enrollStatus;
    QList<UserThumbs> m_thumbsList;
};
}
}

#endif // FINGERMODEL_H
