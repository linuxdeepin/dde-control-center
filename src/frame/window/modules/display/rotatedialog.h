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

#ifndef ROTATEDIALOG_H_V20
#define ROTATEDIALOG_H_V20

#include "../../namespace.h"

#include <QDialog>
#include <QTimer>

#include "DWindowManagerHelper"

namespace dcc {

namespace display {
class Monitor;
class DisplayModel;
}

}

namespace DCC_NAMESPACE {

namespace display {

class RotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotateDialog(dcc::display::Monitor *mon, QWidget *parent = 0);
private:
};

} // namespace display

} // namespace dcc

#endif // ROTATEDIALOG_H
