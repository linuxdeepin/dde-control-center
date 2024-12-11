//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H


#include <QObject>
#include <QQmlEngine>
#include <QDebug>

namespace DCC_NAMESPACE {
class MouseWorker;
class MouseModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit MouseModel(QObject *parent = nullptr);
    ~MouseModel();

    Q_PROPERTY(int scrollSpeed READ scrollSpeed WRITE setScrollSpeed NOTIFY scrollSpeedChanged FINAL)
    Q_PROPERTY(int doubleSpeed READ doubleSpeed WRITE setDoubleSpeed NOTIFY doubleSpeedChanged FINAL)
    Q_PROPERTY(bool leftHandState READ leftHandState WRITE setLeftHandState NOTIFY leftHandStateChanged FINAL)
    Q_PROPERTY(int mouseMoveSpeed READ mouseMoveSpeed WRITE setMouseMoveSpeed NOTIFY mouseMoveSpeedChanged FINAL)
    Q_PROPERTY(bool accelProfile READ accelProfile WRITE setAccelProfile NOTIFY accelProfileChanged FINAL)
    Q_PROPERTY(bool disTpad READ disTpad WRITE setDisTpad NOTIFY disTpadChanged FINAL)
    Q_PROPERTY(bool tpadExist READ tpadExist WRITE setTpadExist NOTIFY tpadExistChanged FINAL)
    Q_PROPERTY(bool mouseNaturalScroll READ mouseNaturalScroll WRITE setMouseNaturalScroll NOTIFY mouseNaturalScrollChanged FINAL)
    Q_PROPERTY(int tpadMoveSpeed READ tpadMoveSpeed WRITE setTpadMoveSpeed NOTIFY tpadMoveSpeedChanged FINAL)
    Q_PROPERTY(bool tapClick READ tapClick WRITE setTapClick NOTIFY tapClickChanged FINAL)
    Q_PROPERTY(bool tpadNaturalScroll READ tpadNaturalScroll WRITE setTpadNaturalScroll NOTIFY tpadNaturalScrollChanged FINAL)
    Q_PROPERTY(bool disIfTyping READ disIfTyping WRITE setDisIfTyping NOTIFY disIfTypingChanged FINAL)

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

    bool tapClick() const { return m_tapClick; }
    void setTapClick(bool tapClick);

    bool tapEnabled() const { return m_touchpadEnabled; }
    void setTapEnabled(bool tapEnabled);

    int scrollSpeed() const { return m_scrollSpeed; }
    void setScrollSpeed(int speed);

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
    void tapClickChanged(bool tapClick);
    void tapEnabledChanged(bool tapClick);
    void scrollSpeedChanged(int speed);
    void disIfTypingChanged(bool state);

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
    bool m_tapClick;
    bool m_touchpadEnabled;
    int  m_doubleSpeed;
    int  m_mouseMoveSpeed;
    int  m_tpadMoveSpeed;
    int  m_redPointMoveSpeed;
    int  m_palmMinWidth;
    int  m_palmMinz;
    int  m_scrollSpeed;
    MouseWorker* m_worker;
};

}



#endif // MOUSEMODEL_H
