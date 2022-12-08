// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H

#include <QObject>
#include <QDebug>

namespace dcc {
namespace mouse {
class MouseModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit MouseModel(QObject *parent = 0);
    ~MouseModel();

    inline bool leftHandState() const { return m_leftHandState; }
    void setLeftHandState(const bool state);

    void setDisIfTyping(const bool state);
    inline bool disIfTyping()   const { return m_disIfTyping; }

    inline bool tpadExist() const { return m_tpadExist; }
    void setTpadExist(bool tpadExist);

    inline bool mouseExist() const { return m_mouseExist; }
    void setMouseExist(bool mouseExist);

    inline bool redPointExist() const { return m_redPointExist; }
    void setRedPointExist(bool redPointExist);

    inline int doubleSpeed() const { return m_doubleSpeed; }
    void setDoubleSpeed(int doubleSpeed);

    inline bool mouseNaturalScroll() const { return m_mouseNaturalScroll; }
    void setMouseNaturalScroll(bool mouseNaturalScroll);

    inline bool tpadNaturalScroll() const { return m_tpadNaturalScroll; }
    void setTpadNaturalScroll(bool tpadNaturalScroll);

    inline int mouseMoveSpeed() const { return m_mouseMoveSpeed; }
    void setMouseMoveSpeed(int mouseMoveSpeed);

    inline int tpadMoveSpeed() const { return m_tpadMoveSpeed; }
    void setTpadMoveSpeed(int tpadMoveSpeed);

    inline bool accelProfile() const { return m_accelProfile; }
    void setAccelProfile(bool useAdaptiveProfile);

    inline bool disTpad() const { return m_disTpad; }
    void setDisTpad(bool disTpad);

    inline int redPointMoveSpeed() const { return m_redPointMoveSpeed; }
    void setRedPointMoveSpeed(int redPointMoveSpeed);

    inline bool palmDetect() const { return m_palmDetect; }
    void setPalmDetect(bool palmDetect);

    inline int palmMinWidth() const { return m_palmMinWidth; }
    void setPalmMinWidth(int palmMinWidth);

    inline int palmMinz() const { return m_palmMinz; }
    void setPalmMinz(int palmMinz);

    bool tapclick() const { return m_tapclick; }
    void setTapClick(bool tapclick);
    bool touchpadEnable() const { return m_touchpadEnable; }
    void setTouchpadEnable(const bool touchpadEnable);
    bool systemTouchpadExist() const { return m_systemTouchpadExist; }
    void setSystemTouchpadExist(const bool exist);

    uint scrollSpeed() const { return m_scrollSpeed; }
    void setScrollSpeed(uint speed);

Q_SIGNALS:
    void leftHandStateChanged(bool state);
    void disIfTypingStateChanged(bool state);
    void tpadExistChanged(bool tpadExist);
    void mouseExistChanged(bool mouseExist);
    void redPointExistChanged(bool rPointExist);
    void doubleSpeedChanged(int speed);
    void mouseNaturalScrollChanged(bool natural);
    void tpadNaturalScrollChanged(bool natural);
    void mouseMoveSpeedChanged(int speed);
    void tpadMoveSpeedChanged(int speed);
    void accelProfileChanged(bool useAdaptiveProfile);
    void redPointMoveSpeedChanged(int speed);
    void disTpadChanged(bool disable);
    void palmDetectChanged(bool detect);
    void palmMinWidthChanged(int palmMinWidth);
    void palmMinzChanged(int palmMinz);
    void tapClickChanged(bool tapclick);
    void touchpadEnableChanged(const bool touchpadEnable);
    void scrollSpeedChanged(uint speed);

private:
    bool m_leftHandState;
    bool m_disIfTyping;
    bool m_tpadExist;
    bool m_mouseExist;
    bool m_redPointExist;
    bool m_mouseNaturalScroll;
    bool m_tpadNaturalScroll;
    bool m_accelProfile;
    bool m_disTpad;
    bool m_palmDetect;
    bool m_tapclick;
    bool m_touchpadEnable;
    bool m_systemTouchpadExist;
    int  m_doubleSpeed;
    int  m_mouseMoveSpeed;
    int  m_tpadMoveSpeed;
    int  m_redPointMoveSpeed;
    int  m_palmMinWidth;
    int  m_palmMinz;
    uint  m_scrollSpeed;
};
}
}



#endif // MOUSEMODEL_H
