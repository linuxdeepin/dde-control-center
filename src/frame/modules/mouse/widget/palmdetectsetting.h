// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
