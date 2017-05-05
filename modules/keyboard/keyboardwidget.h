#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include "modulewidget.h"

#include "keyboardlayoutwidget.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "dccslider.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
class KeyboardModel;
class KeyboardWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(KeyboardModel *model);

signals:
    void keyoard();
    void language();
    void shortcut();
    void delayChanged(int value);
    void speedChanged(int value);
    void capsLockChanged(bool value);
    void numLockChanged(bool value);

public slots:
    void setDelayValue(uint value);
    void setSpeedValue(uint value);

private:
    bool m_bDelay;
    DCCSlider* m_delaySlider;
    DCCSlider* m_speedSlider;
#ifndef DCC_DISABLE_KBLAYOUT
    NextPageWidget* m_keyItem;
#endif
#ifndef DCC_DISABLE_LANGUAGE
    NextPageWidget* m_langItem;
#endif
    NextPageWidget* m_scItem;
    SwitchWidget* m_upper;
    SwitchWidget* m_numLock;
    KeyboardModel *m_model;
};
}
}
#endif // KEYBOARDWIDGET_H
