/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include <QWidget>


namespace DCC_NAMESPACE {
class SwitchWidget;
class CommonInfoModel;

class UserExperienceProgramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserExperienceProgramWidget(QWidget *parent = nullptr);

    void setModel(CommonInfoModel *model);
    void setDefaultUeProgram(const bool enabled);
Q_SIGNALS:
    void enableUeProgram(bool enabled);

private:
    SwitchWidget *m_joinUeProgram;
};
}
