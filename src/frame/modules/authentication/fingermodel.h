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
#include <QMap>

namespace dcc {
namespace authentication {

class FingerModel : public QObject
{
    Q_OBJECT
public:
    enum EnrollResult {
        Enroll_AuthFailed,
        Enroll_ClaimFailed,
        Enroll_Failed,
        Enroll_Success,
        Count
    };

public:
    explicit FingerModel(QObject *parent = nullptr);
    inline QList<QString> getPredefineThumbsName() const { return m_predefineThumbsNames; }

    inline bool isVaild() const {  return m_isVaild; }
    void setIsVaild(bool isVaild);

    inline QString userName() const { return m_userName; }
    void setUserName(const QString &name);

    inline QStringList thumbsList() const { return  m_thumbsList; }
    void setThumbsList(const QStringList &thumbs);

    void onEnrollStatusChanged(int code, const QString& msg);
    void onTouch(const QString &id, bool pressed);

    void resetProgress() { m_progress = 0; }

    void refreshEnrollResult(EnrollResult enrollRes);

Q_SIGNALS:
    void vaildChanged(const bool isVaild);
    void thumbsListChanged(const QStringList &thumbs);

    void enrollFailed(QString title, QString msg);
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollRetry(QString title, QString msg);
    void enrollDisconnected();
    void enrollResult(EnrollResult enrollRes);

    void lockedChanged(bool locked);

private:
    QString m_userName;
    bool m_isVaild{false};
    int m_progress;
    QStringList m_thumbsList;
    QList<QString> m_predefineThumbsNames;
};

}
}

#endif // FINGERMODEL_H
