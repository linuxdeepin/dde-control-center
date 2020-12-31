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

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include <DStandardItem>

#include "interface/namespace.h"
#include "modules/display/displaymodel.h"

class Resolution;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QVBoxLayout;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DStandardItem;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {
class RefreshRateWidget : public QWidget
{
    Q_OBJECT
public:
    RefreshRateWidget(QWidget *parent = nullptr);

    enum ResolutionRole {
        IdRole = Dtk::UserRole,
        WidthRole,
        HeightRole,
        RateRole
    };

public:
    void setModel(dcc::display::DisplayModel *model, dcc::display::Monitor *monitor);
    void setMonitor(dcc::display::Monitor *monitor);

Q_SIGNALS:
    void requestSetResolution(dcc::display::Monitor *monitor, const int mode);

private:
    void initRefreshRate();

private:
    QHBoxLayout *m_contentLayout;
    QLabel *m_refreshLabel;
    QComboBox *m_refreshCombox;

    dcc::display::DisplayModel *m_model;
    dcc::display::Monitor *m_monitor;
    QStandardItemModel *m_refreshItemModel;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // REFRESHRATEWIDGET_H
