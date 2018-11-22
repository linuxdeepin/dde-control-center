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

#ifndef WACOMSETTINGS_H
#define WACOMSETTINGS_H

#include "widgets/translucentframe.h"
#include <QObject>
#include <QVBoxLayout>


class QSlider;
namespace dcc {
namespace widgets {
class SettingsGroup;
class TitledSliderItem;
}
namespace wacom {
class WacomModelBase;
class WacomSettings : public dcc::widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit WacomSettings(QWidget *parent = 0);
    void setModel(WacomModelBase *const baseSettings);

Q_SIGNALS:
    void requestSetPressureValue(const int &value);

public Q_SLOTS:
    void setPressureValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::SettingsGroup *m_mainGroup;
    WacomModelBase *m_baseSettings;
    widgets::TitledSliderItem *m_pressureSlider;
    QSlider *m_preSlider;
};

}
}

#endif // WACOMSETTINGS_H
