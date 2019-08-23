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
#include "modules/defapp/defappworker.h"

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QModelIndex;
class QVBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace defapp {
class DefaultAppsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultAppsWidget(QWidget* parent = nullptr);

Q_SIGNALS:
    void requestCategoryClicked(dcc::defapp::DefAppWorker::DefaultAppsCategory category);

public Q_SLOTS:
    void onCategoryClicked(const QModelIndex &index);

private:
    DTK_WIDGET_NAMESPACE::DListView *m_defAppCatView;
    QVBoxLayout *m_centralLayout;
};
}
}
