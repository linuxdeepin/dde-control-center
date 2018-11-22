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

#ifndef WACOMWIDGET_H
#define WACOMWIDGET_H

#include "modules/modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc{
namespace widgets {
class NextPageWidget;
}
namespace wacom
{
class WacomSettings;
class WacomModel;
class WacomWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit WacomWidget();
    void setModel(WacomModel *const model);

Q_SIGNALS:
    void requestSetPressureValue(const int value);
    void requestShowMode();

private:
    const QString getValue(const bool mode);

private Q_SLOTS:
    void onCursorModeChanged(const bool modeChanged);

private:
    WacomSettings      *m_wacomSettings;
    dcc::widgets::NextPageWidget *m_selectMode;
};
}
}


#endif // WACOMWIDGET_H
