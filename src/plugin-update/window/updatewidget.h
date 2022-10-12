/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "updatectrlwidget.h"
#include "updatemodel.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLayoutItem;
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SettingsGroup;
}

static const QString OfflineUpgraderService = "com.deepin.dde.OfflineUpgrader";

class UpdateCtrlWidget;
class UpdateHistoryButton;
class RecentHistoryApplist;
class AppUpdateInfo;

class UpdateModel;
class UpdateWorker;
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

    void setModel(const UpdateModel *model, const UpdateWorker *work);
    void setSystemVersion(QString version);
    void displayUpdateContent(UpdateType index);

private:
    void showCheckUpdate();
    void showUpdateSetting();

Q_SIGNALS:
    void topListviewChanged(const QModelIndex &index);
    void pushMirrorsView();
    void showUpdateCtrl();
    void requestLastoreHeartBeat();

    void requestUpdates(ClassifyUpdateType type);
    void requestUpdateCtrl(ClassifyUpdateType type, int ctrlType);
    void requestOpenAppStroe();
    void requestFixError(const ClassifyUpdateType &updateType, const QString &error);

public Q_SLOTS:
    void onNotifyUpdateState(int state);

private:
    QVBoxLayout *m_layout;
    UpdateModel *m_model;
    UpdateWorker *m_work;
    QVBoxLayout *m_centerLayout;    
    QLabel *m_label;//System Version display
    QString m_systemVersion;
    QTimer *m_lastoreHeartBeatTimer;            // lastore-daemon 心跳信号，防止lastore-daemon自动退出
    UpdatesStatus m_updateState;
};

