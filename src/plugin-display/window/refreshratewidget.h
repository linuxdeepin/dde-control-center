/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef REFRESHRATEWIDGET_H
#define REFRESHRATEWIDGET_H

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DStandardItem>

class Resolution;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QHBoxLayout;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DStandardItem;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {

class Monitor;
class DisplayModel;

class RefreshRateWidget : public SettingsItem
{
    Q_OBJECT
public:
    RefreshRateWidget(int comboxWidth = 300, QWidget *parent = nullptr);

    enum ResolutionRole { IdRole = Dtk::UserRole, WidthRole, HeightRole, RateRole };

public:
    void setModel(DisplayModel *model, Monitor *monitor);
    void setMonitor(Monitor *monitor);

Q_SIGNALS:
    void requestSetResolution(Monitor *monitor, const int mode);

public Q_SLOTS:
    void OnCurrentModeChanged(const Resolution &mode);

private:
    void initRefreshRate();

private:
    QHBoxLayout *m_contentLayout;
    QLabel *m_refreshLabel;
    QComboBox *m_refreshCombox;

    DisplayModel *m_model;
    Monitor *m_monitor;
    QStandardItemModel *m_refreshItemModel;
};
} // namespace DCC_NAMESPACE
#endif // REFRESHRATEWIDGET_H
