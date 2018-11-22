/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

    inline int screenBlackDelayOnPower() const { return m_screenBlackDelayOnPower; }
    void setScreenBlackDelayOnPower(const int screenBlackDelayOnPower);

    inline int sleepDelayOnPower() const { return m_sleepDelayOnPower; }
    void setSleepDelayOnPower(const int sleepDelayOnPower);

    inline int screenBlackDelayOnBattery() const { return m_screenBlackDelayOnBattery; }
    void setScreenBlackDelayOnBattery(const int screenBlackDelayOnBattery);

    inline int sleepDelayOnBattery() const { return m_sleepDelayOnBattery; }
    void setSleepDelayOnBattery(const int sleepDelayOnBattery);

    inline bool sleepOnLidClose() const { return m_sleepOnLidClose; }
    void setSleepOnLidClose(bool sleepOnLidClose);

#ifndef DCC_DISABLE_POWERSAVE
    inline bool autoPowerSaveMode() const { return m_autoPowerSaveMode; }
    void setAutoPowerSaveMode(bool autoPowerSaveMode);

    inline bool powerSaveMode() const { return m_powerSaveMode; }
    void setPowerSaveMode(bool powerSaveMode);
#endif

    inline bool haveBettary() const { return m_haveBettary; }
    void setHaveBettary(bool haveBettary);

Q_SIGNALS:
    void sleepLockChanged(const bool sleepLock);
    void screenBlackLockChanged(const bool screenBlackLock);
    void lidPresentChanged(const bool lidPresent);
    void sleepOnLidCloseChanged(const bool sleepOnLidClose);
    void screenBlackDelayChangedOnPower(const int screenBlackDelay);
    void sleepDelayChangedOnPower(const int sleepDelay);
    void screenBlackDelayChangedOnBattery(const int screenBlackDelay);
    void sleepDelayChangedOnBattery(const int sleepDelay);
#ifndef DCC_DISABLE_POWERSAVE
    void autoPowerSavingModeChanged(bool autoPowerSaveMode);
    void powerSaveModeChanged(bool powerSaveMode);
#endif
    void haveBettaryChanged(bool haveBettary);

private:
    bool m_lidPresent;
    bool m_sleepOnLidClose;
    bool m_screenBlackLock;
    bool m_sleepLock;
    int m_screenBlackDelayOnPower;
    int m_sleepDelayOnPower;
    int m_screenBlackDelayOnBattery;
    int m_sleepDelayOnBattery;
#ifndef DCC_DISABLE_POWERSAVE
    bool m_autoPowerSaveMode;
    bool m_powerSaveMode;
#endif
    bool m_haveBettary;
};

}
}


#endif // POWERMODEL_H
