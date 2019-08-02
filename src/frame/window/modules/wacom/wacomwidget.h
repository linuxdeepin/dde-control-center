/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#ifndef WACOMWIDGET_H_V20
#define WACOMWIDGET_H_V20

#include "modules/modulewidget.h"
#include "window/namespace.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc {
namespace wacom {
class WacomSettings;
class WacomModel;
}
namespace widgets {
class NextPageWidget;
}
}
namespace DCC_NAMESPACE {
namespace wacom {

class WacomWidget : public QWidget
{
    Q_OBJECT
private:
    enum Mode {
        MOUSE,
        PEN
    };

public:
    explicit WacomWidget(QWidget *parent = 0);
    void setModel(dcc::wacom::WacomModel *const model);

Q_SIGNALS:
    void requestSetPressureValue(const int value);
    void requestShowMode();

private:
    QString getValue(const Mode mode) const;

private Q_SLOTS:
    void onCursorModeChanged(const bool modeChanged);

private:
    dcc::wacom::WacomSettings *m_Sensitivity;
    dcc::widgets::NextPageWidget *m_selectMode;
    QVBoxLayout *m_centralLayout;
};
}
}


#endif // WACOMWIDGET_H_V20
