//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    void updateSystemVersionLabel();
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

