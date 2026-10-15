// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WIRELESSCASTINGAPPLET_H
#define WIRELESSCASTINGAPPLET_H

#include "widget/commoniconbutton.h"
#include "wirelesscastingmodel.h"
#include "widget/monitorlistview.h"
#include "widget/jumpsettingbutton.h"
#include "displaymodel.h"

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>

#include <DIconButton>

DWIDGET_BEGIN_NAMESPACE
class DLabel;
DWIDGET_END_NAMESPACE

using namespace Dtk::Widget;

namespace dde {
namespace wirelesscasting {

class StatePanel;
class WirelessCastingApplet : public QWidget
{
    Q_OBJECT
public:
    explicit WirelessCastingApplet(WirelessCastingModel *model, DisplayModel *displayMode, QWidget *parent = nullptr);

    void resizeApplet();
    void setMinHeight(int minHeight);
    void onContainerChanged(int container);
#ifdef WIRLESS_CASTING_ENABLED
    QWidget *refreshButton() { return m_refresh; };
#endif

public slots:
    void onStateChanged(WirelessCastingModel::CastingState status);
    void onAddMonitor(const QString &path, Monitor *monitor);
    void onRemoveMonitor(const QString &path);
    bool casting() const;

signals:
    void castingChanged(bool casting);
    void requestHideApplet();

private:
    void initUI();
    void initMonitors();

private:
    WirelessCastingModel *m_model;
    DisplayModel *m_displayModel;

    QWidget *m_contentWidget;
    QScrollArea *m_scrollArea;
#ifdef WIRELESS_CASTING_ENABLED
    CommonIconButton *m_refresh;
    QWidget *m_wirelesscastingWidget;
    QWidget *m_wirelesscastingTitle;
    MonitorListView *m_monitorsListView;
    QStandardItemModel *m_monitorsModel;
    StatePanel *m_statePanel;
#endif
    QWidget *m_multiscreenOptionsWidget;
    QWidget *m_multiscreenOptionsTitle;
    MonitorListView *m_multiscreenOptionsListView;
    QStandardItemModel *m_multiscreenOptionsModel;
    bool m_showOnDock;

    JumpSettingButton *m_displaySetting;

    // 包含MonitorItem*, Monitor*的结构体
    typedef struct _MonitorMapItem {
        MonitorItem *item;
        Monitor *monitor;
    } MonitorMapItem;
    QMap<QString, MonitorMapItem> m_monitors;
    Monitor *m_lastConnMonitor;
    bool m_cfgShowCasting;
    bool m_cfgEnabled;
    int m_minHeight;
};

class StatePanel : public QWidget
{
    Q_OBJECT
public:
    enum  ConnectionState{
        Connected = 1,
        NoMonitor,
        WarningInfo,
        DisabledWirelessDevice,
        NoWirelessDevice,
        Count
    };

    explicit StatePanel(WirelessCastingModel *model, QWidget *parent = nullptr);
    ~StatePanel() Q_DECL_OVERRIDE;
    int infoHeight();

public slots:
    void setState(WirelessCastingModel::CastingState status);

signals:
    void disconnMonitor();

private:
    WirelessCastingModel *m_model;
    QWidget *m_infoWidget;
    DLabel *m_infoLab;
    QWidget *m_noDeviceBox;
    DLabel *m_iconNoDevice;
    DLabel *m_noDevice;
    QWidget *m_connectedDeviceBox;
    DLabel *m_iconConnected;
    DLabel *m_connected;
    CancelButton *m_disconnMonitor;
};
} // namespace wirelesscasting
} // namespace dde

#endif // WIRELESSCASTINGAPPLET_H
