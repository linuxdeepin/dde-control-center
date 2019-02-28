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

#ifndef RESOLUTIONDETAILPAGE_H
#define RESOLUTIONDETAILPAGE_H

#include "widgets/contentwidget.h"
#include "widgets/optionitem.h"

#include <QMap>

namespace dcc {

namespace widgets {
class SettingsGroup;
}

namespace display {

class Monitor;
class DisplayModel;
class ResolutionDetailPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ResolutionDetailPage(QWidget *parent = 0);

    void setModel(DisplayModel *model);

Q_SIGNALS:
    void requestSetResolution(Monitor *mon, const int mode) const;

private Q_SLOTS:
    void onItemClicked();
    void refreshCurrentResolution();

private:
    DisplayModel *m_model;
    dcc::widgets::SettingsGroup *m_resolutions;
    dcc::widgets::OptionItem *m_currentItem;

    QMap<dcc::widgets::OptionItem *, int> m_options;
};

}   // namespace dcc

}   // namespace display

#endif // RESOLUTIONDETAILPAGE_H
