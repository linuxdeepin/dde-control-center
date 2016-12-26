#ifndef THINKPADSETTINGS_H
#define THINKPADSETTINGS_H


#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>

class QSlider;
namespace dcc
{

namespace widgets
{
class TitledSliderItem;
class SettingsGroup;
class SettingsHead;
}
namespace mouse
{
class MouseModelThinkpadSettings;
class ThinkpadSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit ThinkpadSettings(QWidget *parent = 0);
    void setModel(MouseModelThinkpadSettings *const baseSettings);

signals:
    void requestSetSliderValue(const int &value);

public slots:
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    widgets::SettingsGroup *m_mainGroup;
    MouseModelThinkpadSettings *m_baseSettings;
    widgets::TitledSliderItem *m_speedSlider;
    widgets::SettingsHead *m_title;
};
}
}


#endif // THINKPADSETTINGS_H
