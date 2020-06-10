/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     dr <dongrui@deepin.com>
 *
 * Maintainer: dr <dongrui@deepin.com>
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

#ifndef MULTISCREENDETAILPAGE_H
#define MULTISCREENDETAILPAGE_H

#include "interface/namespace.h"
#include "widgets/titlevalueitem.h"
#include <QVBoxLayout>

#include <QWidget>

namespace dcc {
namespace display {
class DisplayModel;
}
}

namespace DCC_NAMESPACE {

namespace display {

class MultiScreenDetailPage : public QWidget
{
    Q_OBJECT
public:
    explicit MultiScreenDetailPage(bool type, QWidget *parent = nullptr);

public:
    void setModel(dcc::display::DisplayModel *model);

public Q_SLOTS:

Q_SIGNALS:

private:
    void initUI();

private:
    dcc::display::DisplayModel *m_model{nullptr};
    bool m_refreshOrResolution;
    QVBoxLayout *m_mainLayout;
};

}

}

#endif // MULTISCREENDETAILPAGE_H
