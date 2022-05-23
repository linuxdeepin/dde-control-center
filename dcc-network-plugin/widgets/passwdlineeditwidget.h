/*
 * Copyright (C) 2016 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     yinjie <yinjie@uniontech.com>

 * Maintainer: yinjie <yinjie@uniontech.com>
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

#ifndef __DPASSWORDEDITEX__H__
#define __DPASSWORDEDITEX__H__


#include <widgets/lineeditwidget.h>

#include <DPasswordEdit>

using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

class PasswdLineEditWidget : public LineEditWidget
{
    Q_OBJECT
public:
    explicit PasswdLineEditWidget(QWidget *parent = nullptr);

    void setText(const QString &text);
};

#endif  //!__DPASSWORDEDITEX__H__
