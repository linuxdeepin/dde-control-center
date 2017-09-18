/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef POWERMODEL_H
#define POWERMODEL_H

#include <QObject>

namespace dcc {
namespace power {

class PowerWorker;
class PowerModel : public QObject
{
    Q_OBJECT

    friend class PowerWorker;

public:
    explicit PowerModel(QObject *parent = 0);

    inline bool screenBlackLock() const { return m_screenBlackLock; }
    void setScreenBlackLock(const bool lock);

    inline bool sleepLock() const { return m_sleepLock; }
    void setSleepLock(bool sleepLock);

    inline bool lidPresent() const { return m_lidPresent; }
    void setLidPresent(bool lidPresent);

    inline int screenBlackDelay() const { return m_screenBlackDelay; }
    void setScreenBlackDelay(const int screenBlackDelay);

    inline int sleepDelay() const { return m_sleepDelay; }
    void setSleepDelay(const int sleepDelay);

    inline bool sleepOnLidClose() const { return m_sleepOnLidClose; }
    void setSleepOnLidClose(bool sleepOnLidClose);

signals:
    void sleepLockChanged(const bool sleepLock);
    void screenBlackLockChanged(const bool screenBlackLock);
    void lidPresentChanged(const bool lidPresent);
    void sleepOnLidCloseChanged(const bool sleepOnLidClose);
    void screenBlackDelayChanged(const int screenBlackDelay);
    void sleepDelayChanged(const int sleepDelay);

private:
    bool m_lidPresent;
    bool m_sleepOnLidClose;
    bool m_screenBlackLock;
    bool m_sleepLock;
    int m_screenBlackDelay;
    int m_sleepDelay;
};

}
}


#endif // POWERMODEL_H
