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

#ifndef MIRACASTNODEVICEWIDGET_H
#define MIRACASTNODEVICEWIDGET_H

#include "widgets/translucentframe.h"
#include "widgets/loadingindicator.h"
#include "widgets/labels/normallabel.h"
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace display {

class MiracastNoDeviceWidget : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit MiracastNoDeviceWidget(QWidget *parent = 0);

    enum DeviceListState{
        NoDevice,
        Refreshed
    };

Q_SIGNALS:
    void requestRefreshed() const;

public Q_SLOTS:
    void setState(DeviceListState state);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void refresh();
    void nodevice();

private:
    LoadingIndicator *m_loading;
    NormalLabel *m_refreshTip;
    NormalLabel *m_nodeviceTip;
    DImageButton *m_refreshBtn;
};

}
}

#endif // MIRACASTNODEVICEWIDGET_H
