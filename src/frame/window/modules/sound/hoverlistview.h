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

#ifndef HOVERLISTVIEW_H
#define HOVERLISTVIEW_H

#include "window/namespace.h"

#include <QListView>

namespace DCC_NAMESPACE {

namespace sound {
class HoverListView : public QListView
{
public:
    HoverListView(QWidget *parent = nullptr);

protected:
    void leaveEvent(QEvent *event) override;
    bool viewportEvent(QEvent *event) override;
};
}
}

#endif // HOVERLISTVIEW_H
