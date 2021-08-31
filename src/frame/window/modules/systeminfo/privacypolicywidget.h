/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     longqi <longqi@uniontech.com>
*
* Maintainer: longqi <longqi@uniontech.com>
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

namespace DCC_NAMESPACE {
namespace systeminfo {

class PrivacyPolicyWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit PrivacyPolicyWidget(QWidget *parent = nullptr);

public Q_SLOTS:
    void onLinkActivated(const QString link);
};

}
}
