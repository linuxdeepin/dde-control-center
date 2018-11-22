/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#ifndef PALMDETECTSETTING_H
#define PALMDETECTSETTING_H

#include "widgets/translucentframe.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"

#include <QWidget>

namespace dcc {
namespace mouse {
class MouseModel;
class PalmDetectSetting : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit PalmDetectSetting(QWidget *parent = nullptr);

    void setModel(MouseModel * const model);

Q_SIGNALS:
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);

private Q_SLOTS:
    void setDetectState(bool enable);
    void setContactValue(int value);
    void setPressureValue(int value);

private:
    widgets::SwitchWidget *m_detectSwitchBtn;
    widgets::TitledSliderItem *m_contactSlider;
    widgets::TitledSliderItem *m_pressureSlider;
};
}
}

#endif // PALMDETECTSETTING_H
