// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include "mousemodel.h"

#include <QObject>


namespace dcc {
namespace mouse {
class MouseWorker : public QObject
{
    Q_OBJECT
public:
    explicit MouseWorker(MouseModel *model, QObject *parent = 0);
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
}



#endif // MOUSEWORKER_H
