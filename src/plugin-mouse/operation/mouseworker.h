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

#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include "interface/namespace.h"
#include "mousemodel.h"

#include <QObject>


namespace DCC_NAMESPACE {
class MouseWorker : public QObject
{
    Q_OBJECT
public:
    explicit MouseWorker(MouseModel *model, QObject *parent = nullptr);
    void active();
    void deactive();
    void init();

public Q_SLOTS:
    void setMouseExist(bool exist);
    void setTpadExist(bool exist);
    void setRedPointExist(bool exist);
    void setLeftHandState(const bool state);
    void setMouseNaturalScrollState(const bool state);
    void setTouchNaturalScrollState(const bool state);
    void setDisTyping(const bool state);
    void setDisTouchPad(const bool state);
    void setTapClick(const bool state);
    void setDouClick(const int &value);
    void setMouseMotionAcceleration(const double &value);
    void setAccelProfile(const bool state);
    void setTouchpadMotionAcceleration(const double &value);
    void setTrackPointMotionAcceleration(const double &value);
    void setPalmDetect(bool palmDetect);
    void setPalmMinWidth(int palmMinWidth);
    void setPalmMinz(int palmMinz);
    void setScrollSpeed(uint speed);

    void onLeftHandStateChanged(const bool state);
    void onMouseNaturalScrollStateChanged(const bool state);
    void onTouchNaturalScrollStateChanged(const bool state);
    void onDisTypingChanged(const bool state);
    void onDisTouchPadChanged(const bool state);
    void onTapClick(const bool state);
    void onDouClickChanged(const int &value);
    void onMouseMotionAccelerationChanged(const int &value);
    void onAccelProfileChanged(const bool state);
    void onTouchpadMotionAccelerationChanged(const int &value);
    void onTrackPointMotionAccelerationChanged(const int &value);
    void onPalmDetectChanged(bool palmDetect);
    void onPalmMinWidthChanged(int palmMinWidth);
    void onPalmMinzChanged(int palmMinz);
    void onScrollSpeedChanged(int speed);

Q_SIGNALS:
    void requestSetPalmDetect(bool palmDetect);
    void requestSetPalmMinWidth(int palmMinWidth);
    void requestSetPalmMinz(int palmMinz);
    void requestSetScrollSpeed(uint speed);
    void requestSetLeftHandState(const bool state);
    void requestSetMouseNaturalScrollState(const bool state);
    void requestSetTouchNaturalScrollState(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetTapClick(const bool state);
    void requestSetDouClick(const int &value);
    void requestSetMouseMotionAcceleration(const double &value);
    void requestSetAccelProfile(const bool state);
    void requestSetTouchpadMotionAcceleration(const double &value);
    void requestSetTrackPointMotionAcceleration(const double &value);

private:
    int converToDouble(int value);
    int converToDoubleModel(int value);
    double converToMotionAcceleration(int value);
    int converToModelMotionAcceleration(double value);

private:
    MouseModel *m_model;
};
}



#endif // MOUSEWORKER_H
