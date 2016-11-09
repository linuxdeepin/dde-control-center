#ifndef MOUSEWIDGET_H
#define MOUSEWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "widget/basesettings.h"
#include  "widget/mousesettings.h"
#include "widget/thinkpadsettings.h"
#include "mousemodel.h"
#include <QPushButton>
#include <QVBoxLayout>

class MouseWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit MouseWidget();
    void setModel(MouseModel * const model);

signals:
    void setDefault();
    void requestSetLeftHand(const bool state);
    void requestSetNaturalScroll(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetTapClick(const bool state);
    void requestSetDouClick(const int &value);
    void requestSetMouseMotionAcceleration(const int &value);
    void requestSetTouchpadMotionAcceleration(const int &value);
    void requestSetTrackPointMotionAcceleration(const int &value);

private:
    dcc::SettingsGroup *m_userGroup;
    QPushButton *m_nextPage;
    BaseSettings *m_baseSettings;
    MouseSettings *m_mouseSettings;
    MouseSettings *m_touchSettings;
    ThinkpadSettings *m_ThinkapdSettings;
};

#endif // MOUSEWIDGET_H
