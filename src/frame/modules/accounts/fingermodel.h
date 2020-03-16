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

    bool isVaild() const;
    void setIsVaild(bool isVaild);

    void setThumbsList(const QStringList &thumbs);
    QStringList thumbsList() const;

    void onEnrollStatusChanged(int code, const QString& msg);
    void onTouch(const QString &id, bool pressed);
Q_SIGNALS:
    void vaildChanged(const bool isVaild);
    void thumbsListChanged(const QStringList &thumbs);

    void enrollFailed(QString msg);
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollRetry(QString msg);
    void enrollDisconnected();

    void lockedChanged(bool locked);
private:
    bool m_isVaild{false};
    QList<QString> m_thumbsList;
};
}
}

#endif // FINGERMODEL_H
