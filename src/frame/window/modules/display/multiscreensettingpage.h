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

#ifndef MULTISCREENSETTINGPAGE_H
#define MULTISCREENSETTINGPAGE_H

#include "../../namespace.h"

#include <QWidget>
class QModelIndex;

namespace dcc{

namespace display{
    class DisplayModel;
}

namespace widgets{
    class BasicListView;
}

}

namespace DCC_NAMESPACE {

namespace display {

class MultiScreenSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit MultiScreenSettingPage(QWidget* parent = nullptr);
public:
    void setModel(dcc::display::DisplayModel *model);
Q_SIGNALS:
    void requestDuplicateMode();
    void requestExtendMode();
    void requestCustom();
    void requestOnlyMonitor(const QString&);
    void requestConfig(const QString&);
private Q_SLOTS:
    void onItemClicked(const QModelIndex&);
private:
    dcc::widgets::BasicListView *m_baseListView{nullptr};
};

}

}

#endif // MULTISCREENSETTINGPAGE_H
