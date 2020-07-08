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

#ifndef REFRESHRATEPAGE_H
#define REFRESHRATEPAGE_H

#include "interface/namespace.h"

#include <QWidget>
#include <QVBoxLayout>

namespace dcc {
namespace display {
class DisplayModel;
class Monitor;
}
}

namespace DCC_NAMESPACE{

namespace display {
class RefreshRatePage: public QWidget
{
    Q_OBJECT
public:
    RefreshRatePage(QWidget* parent = nullptr);

public:
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestSetResolution(dcc::display::Monitor *, int);

private:
    void initRateList();

private:
    dcc::display::DisplayModel *m_model{nullptr};
    dcc::display::Monitor *m_monitor{nullptr};
    QVBoxLayout *m_layout{nullptr};
};
}

}


#endif // REFRESHRATEPAGE_H
