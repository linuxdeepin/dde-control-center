/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include <DListView>

namespace DCC_NAMESPACE {
namespace defapp {
class DefAppListView : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT

public:
    DefAppListView(QWidget *parent = Q_NULLPTR);

    enum DefAppDataRole{
        DefAppIsUserRole = Dtk::UserRole + 1,
        DefAppIdRole,
    };

    const QModelIndex &currentHoverIndex() const;

Q_SIGNALS:
    void currentHoverChanged(const QModelIndex &previous, const QModelIndex &current);

protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onCurrentHoverChanged(const QModelIndex &previous, const QModelIndex &current);
    void onItemEntered(const QModelIndex &index);

private:
    QModelIndex m_indexPrevious;
    QModelIndex m_indexCurrent;
};
}
}



