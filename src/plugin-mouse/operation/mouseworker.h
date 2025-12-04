//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include "mousemodel.h"
#include "treelandworker.h"

#include <QObject>

using namespace DCC_MOUSE;
namespace DCC_NAMESPACE {
class MouseWorker : public QObject
{
    Q_OBJECT
public:
    explicit MouseWorker(MouseModel *model, QObject *parent = nullptr);
    void active();
    void deactive();
    void init();

    void initFingerGestures();

public Q_SLOTS:
    void setMouseExist(bool exist);
    void setTpadExist(bool exist);
    void setTpadEnabled(bool enabled);
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
    void setGestureData(const GestureData &data);
    void setCursorSize(const int cursorSize);
    void setAvailableCursorSizes(QList<int> sizes);

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
    void onTouchpadEnabledChanged(const bool state);
    void onCursorSizeChanged(const int cursorSize);

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
    void requestSetTouchpadEnabled(const bool state);
    void requestSetGesture(const QString& name, const QString& direction, int fingers, const QString& action);
    void requestSetCursorSize(const int cursorSize);

private:
    int converToDouble(int value);
    int converToDoubleModel(int value);
    double converToMotionAcceleration(int value);
    int converToModelMotionAcceleration(double value);

private:
    MouseModel *m_model;
    TreeLandWorker *m_treelandWorker;
};
}

#endif // MOUSEWORKER_H
