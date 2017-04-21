#ifndef BASESETTINGS_H
#define BASESETTINGS_H

#include "translucentframe.h"
#include <QVBoxLayout>

namespace dcc
{
namespace widgets
{
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
class TranslucentFrame;
}
namespace mouse
{
class MouseModelBaseSettings;
class BaseSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit BaseSettings(QWidget *parent = 0);
    void setIsTypingVisible(const bool visible);

signals:
    void Reset();
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetSliderValue(const int &value);

public slots:
    void setModel(MouseModelBaseSettings *const baseSettings);
    void setSliderValue(const int &value);

private:
    QVBoxLayout   *m_mainLayout;
    widgets::SettingsGroup *m_mainGroup;
    widgets::SwitchWidget  *m_leftHand;
    widgets::SwitchWidget  *m_isTyping;
    widgets::TitledSliderItem *douSlider;
    MouseModelBaseSettings *m_baseSettings;
};
}
}


#endif // BASESETTINGS_H
