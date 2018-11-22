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

#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include "widgets/loadingindicator.h"
#include <dimagebutton.h>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
class ConnectWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ConnectWidget(QWidget *parent = 0);

    enum ConnectState {
        Connected,
        Connecting,
        ConnectFaild
    };

Q_SIGNALS:
    void clicked();

public Q_SLOTS:
    void onConnectChanged(ConnectState state);
    void setMouseEnter(const bool state);

private:
    ConnectState m_state;
    DImageButton *m_disconnectBtn;
    DImageButton *m_connectedBtn;
    LoadingIndicator *m_loading;
};

}
}

#endif // CONNECTWIDGET_H
