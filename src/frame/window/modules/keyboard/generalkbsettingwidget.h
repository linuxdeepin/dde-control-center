/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#ifndef GENERALKBSETTINGWIDGET_H
#define GENERALKBSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include <QMouseEvent>
#include <QLineEdit>

namespace dcc {
namespace keyboard {
class KeyboardModel;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class DCCSlider;
class GLineEdit;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class GeneralKBSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit GeneralKBSettingWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = nullptr);
    class GLineEdit: public QLineEdit
    {
        void focusOutEvent(QFocusEvent *) override
        {
            clear();
            update();
        }
    };
Q_SIGNALS:
    void requestKBDelayChanged(const int value);
    void requestKBSpeedChanged(const int value);
    void requestNumLockChanged(const bool state);
    void requestCapsLockChanged(const bool state);
private Q_SLOTS:
    void setDelayValue(uint value);
    void setSpeedValue(uint value);
private:
    dcc::widgets::DCCSlider *m_delaySlider;
    dcc::widgets::DCCSlider *m_speedSlider;
    dcc::widgets::SwitchWidget *m_upper;
    dcc::widgets::SwitchWidget *m_numLock;
    dcc::widgets::SettingsGroup *m_generalSettingsGrp;
    dcc::keyboard::KeyboardModel *m_model;
    QVBoxLayout *m_contentLayout;
    GLineEdit *m_testArea;
protected:
    void mousePressEvent(QMouseEvent *event);
};
}
}
#endif // GENERALKBSETTINGWIDGET_H
