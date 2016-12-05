#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include "modulewidget.h"

#include "keyboardlayoutwidget.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "dccslider.h"

using namespace dcc;

class KeyboardWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget();

signals:
    void keyoard();
    void language();
    void shortcut();
    void delayChanged(int value);
    void speedChanged(int value);

public slots:
    void setKBValue(const QString& value);
    void setLangValue(const QString& value);
    void onSliderValue(int value);
    void onTimeout();
    void setDelayValue(uint value);
    void setSpeedValue(uint value);

private:
    bool m_bDelay;
    QTimer *m_timer;
    DCCSlider* m_delaySlider;
    DCCSlider* m_speedSlider;
    NextPageWidget* m_keyItem;
    NextPageWidget* m_langItem;
    NextPageWidget* m_scItem;
    SwitchWidget* m_upper;
};

#endif // KEYBOARDWIDGET_H
