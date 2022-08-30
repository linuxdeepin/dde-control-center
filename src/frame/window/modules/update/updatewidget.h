// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "updatectrlwidget.h"

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QList>

#include <DButtonBox>

class AppUpdateInfo;

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLayoutItem;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateModel;
class UpdateWorker;
}

namespace widgets {
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace update {

static const QString OfflineUpgraderService = "com.deepin.dde.OfflineUpgrader";

class UpdateCtrlWidget;
class UpdateHistoryButton;
class RecentHistoryApplist;
class UpdateSettings;

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    enum UpdateType {
        Default = -1,
        UpdateCheck,
        UpdateSetting,
        UpdateSettingMir,
        Count
    };
public:
    explicit UpdateWidget(QWidget *parent = nullptr);
    ~UpdateWidget();

    void initialize();
    void setModel(const UpdateModel *model, const UpdateWorker *work);
    void setSystemVersion(QString version);
    void resetUpdateCheckState(bool state = true);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void refreshWidget(UpdateType type = UpdateCheck);

private:
    void displayUpdateContent(UpdateType index);
    void showCheckUpdate();
    void showUpdateSetting();

Q_SIGNALS:
    void topListviewChanged(const QModelIndex &index);
    void pushMirrorsView();
    void showUpdateCtrl();
    void requestLastoreHeartBeat();

public Q_SLOTS:
    void onNotifyUpdateState(int state);
    void onAppendApplist(const QList<AppUpdateInfo> &infos);

private:
    QVBoxLayout *m_layout;
    UpdateModel *m_model;
    UpdateWorker *m_work;
    QVBoxLayout *m_centerLayout;
    QLabel *m_label;//System Version display
    QString m_systemVersion;
    UpdateHistoryButton *m_historyBtn;//update history button
    UpdatesStatus m_updateState;
    QLabel *m_updateHistoryText;
    dcc::widgets::SettingsGroup *m_applistGroup;
    RecentHistoryApplist *m_recentHistoryApplist;
    DButtonBox *m_topSwitchWidgetBtn;
    QList<DButtonBoxButton *> m_btnlist;
    QStackedLayout *m_mainLayout;
    QTimer *m_lastoreHeartBeatTimer;            // lastore-daemon 心跳信号，防止lastore-daemon自动退出
};

}// namespace datetime
}// namespace DCC_NAMESPACE
