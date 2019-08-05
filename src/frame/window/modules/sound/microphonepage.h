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

#ifndef MICROPHONEPAGE_H_V20
#define MICROPHONEPAGE_H_V20

#include "window/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {
class TitledSliderItem;
class SwitchWidget;
}

namespace sound {
class SoundModel;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class MicrophonePage : public QWidget
{
    Q_OBJECT
public:
    MicrophonePage(QWidget *parent = nullptr);

    ~MicrophonePage();
public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchMicrophone(bool on);
    void requestSetMicrophoneVolume(double vol);

private:
    void initSlider();

private:
    dcc::sound::SoundModel *m_model{nullptr};
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::SwitchWidget *m_sw{nullptr};
    dcc::widgets::TitledSliderItem *m_inputSlider{nullptr};
    dcc::widgets::TitledSliderItem *m_feedbackSlider{nullptr};
    QMetaObject::Connection m_conn;

};

}
}

#endif // MICROPHONEPAGE_H_V20
