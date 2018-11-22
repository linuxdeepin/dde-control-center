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

#ifndef DISPLAYCONTROLPAGE_H
#define DISPLAYCONTROLPAGE_H

#include "displaymodel.h"
#include "displayworker.h"
#include "widgets/settingsitem.h"

#include <QModelIndex>
#include <QWidget>

namespace dcc {
namespace widgets {
class BasicListView;
}
namespace display {
class DisplayControlPage : public widgets::SettingsItem {
    Q_OBJECT

public:
    explicit DisplayControlPage(QWidget *parent = 0);
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestDuplicateMode() const;
    void requestExtendMode() const;
    void requestOnlyMonitor(const QString &monName) const;
    void requestConfig(const QString &config) const;
    void requestCustom() const;

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index) const;

private:
    widgets::BasicListView *m_baseListView;
};
}
}

#endif  // DISPLAYCONTROLPAGE_H
