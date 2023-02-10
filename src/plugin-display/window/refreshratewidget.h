//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


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

    enum ResolutionRole {
        IdRole = Dtk::UserRole,
        WidthRole,
        HeightRole,
        RateRole
    };

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
}
#endif // REFRESHRATEWIDGET_H
