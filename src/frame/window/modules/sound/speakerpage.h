/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef SPEAKERPAGE_H_V20
#define SPEAKERPAGE_H_V20

#include "../../namespace.h"

#include <QWidget>

class QVBoxLayout;

namespace dcc {

namespace sound {
class SoundModel;
}

namespace widgets {
class TitledSliderItem;
class SwitchWidget;
}

}

namespace DCC_NAMESPACE {

namespace sound {

class SpeakerPage : public QWidget
{
    Q_OBJECT
public:
    SpeakerPage(QWidget *parent = nullptr);
    ~SpeakerPage();
public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchSpeaker(bool isOpen);
    void requestSetSpeakerVolume(double val);
    void requestSetSpeakerBalance(double val);

private:
    void initSlider();

private:
    dcc::sound::SoundModel *m_model{nullptr};
    dcc::widgets::SwitchWidget *m_sw{nullptr};
    dcc::widgets::TitledSliderItem *m_outputSlider{nullptr};
    dcc::widgets::TitledSliderItem *m_balanceSlider{nullptr};
    QVBoxLayout *m_layout{nullptr};
};

}
}

#endif // SPEAKERPAGE_H_V20
