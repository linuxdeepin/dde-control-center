/*
 * Copyright (C) 2011 ~ 2019 Uniontech Technology Co., Ltd.
 *
 * Author:     wangwei <wangwei@uniontech.com>
 *
 * Maintainer: wangwei <wangwei@uniontech.com>
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

// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef ACCOUNTS_ONLINE_DELEGATE_H
#define ACCOUNTS_ONLINE_DELEGATE_H

#include "interface/namespace.h"

#include <QWidget>
#include <QGraphicsDropShadowEffect>

namespace DCC_NAMESPACE {

class OnlineIcon : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineIcon(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QGraphicsDropShadowEffect *m_shadowEffect;
};
}

#endif  // ACCOUNTS_ONLINE_DELEGATE_H
