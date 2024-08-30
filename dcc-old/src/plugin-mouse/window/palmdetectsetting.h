//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PALMDETECTSETTING_H
#define PALMDETECTSETTING_H

#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class MouseModel;
class PalmDetectSetting : public QFrame
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
    SwitchWidget *m_detectSwitchBtn;
    TitledSliderItem *m_contactSlider;
    TitledSliderItem *m_pressureSlider;
};
}

#endif // PALMDETECTSETTING_H
