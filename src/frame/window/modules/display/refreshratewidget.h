// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

using namespace dcc::widgets;

namespace DCC_NAMESPACE {

namespace display {

class RefreshRateWidget : public SettingsItem
{
    Q_OBJECT
public:
    RefreshRateWidget(int comboxWidth = 300, QWidget *parent = nullptr);

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

public Q_SLOTS:
    void OnCurrentModeChanged(const Resolution &mode);

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
