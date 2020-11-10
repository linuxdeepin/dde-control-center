/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <QWidget>

namespace  dcc {
namespace widgets {
class TipsLabel;
}
}

namespace DCC_NAMESPACE {
namespace systeminfo {

class UserLicenseWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit UserLicenseWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void loadTextFinished();

private:
    dcc::widgets::TipsLabel *m_body;
};

}
}
