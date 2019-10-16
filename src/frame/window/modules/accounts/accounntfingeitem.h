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

#include "window/namespace.h"
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE

class QWidget;
class QHBoxLayout;
class QStackedWidget;
class QLabel;
class QLineEdit;

QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

class AccounntFingeItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit AccounntFingeItem(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void appendItem(QWidget *widget);

Q_SIGNALS:
    void deleteItem();

private:
    QHBoxLayout *m_layout;
    QLabel *m_title;
};

}
}
