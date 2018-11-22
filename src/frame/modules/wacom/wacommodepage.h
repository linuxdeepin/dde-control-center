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

#ifndef WACOMMODEPAGE_H
#define WACOMMODEPAGE_H

#include "widgets/contentwidget.h"
#include <QWidget>

namespace dcc {
namespace widgets {
class OptionItem;
}
namespace wacom {

class WacomModePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit WacomModePage(QWidget *parent = nullptr);

    void setMode(const bool mode);

Q_SIGNALS:
    void requestSetMode(const bool mode);

private Q_SLOTS:
    void modeSelected();

private:
    dcc::widgets::OptionItem *m_penMode;
    dcc::widgets::OptionItem *m_mouseMode;
};
}
}


#endif // WACOMMODEPAGE_H
