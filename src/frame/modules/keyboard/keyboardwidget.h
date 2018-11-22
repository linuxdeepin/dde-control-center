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

#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include "modules/modulewidget.h"

#include "keyboardlayoutwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/dccslider.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
class KeyboardModel;
class KeyboardWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(KeyboardModel *model);

Q_SIGNALS:
    void keyoard();
    void language();
    void shortcut();
    void delayChanged(int value);
    void speedChanged(int value);
    void capsLockChanged(bool value);
    void numLockChanged(bool value);

public Q_SLOTS:
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
