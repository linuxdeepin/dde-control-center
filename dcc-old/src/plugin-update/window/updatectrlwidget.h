//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/updatesettingitem.h"
#include "widgets/utils.h"
#include "common.h"


#include <QWidget>
#include <dtkwidget_global.h>

QT_BEGIN_NAMESPACE
class QSettings;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SettingsGroup;
}

DWIDGET_BEGIN_NAMESPACE
class DSpinner;
class DLabel;
DWIDGET_END_NAMESPACE

class LoadingItem;
class AppUpdateInfo;
class SystemUpdateItem;
class SafeUpdateItem;
class UnknownUpdateItem;
class DownloadProgressBar;
class UpdateItemInfo;
class ResultItem;
class SafeUpdateItem;
class LoadingItem;
class SummaryItem;
class SystemUpdateItem;
class UnknownUpdateItem;
class UpdateModel;
class UpdateCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateCtrlWidget(UpdateModel *model, QWidget *parent = 0);
    ~UpdateCtrlWidget();

    void initConnect();
    void initModel();
    void updateSystemVersionLabel();

    UpdateErrorType updateJobErrorMessage() const;
    void setUpdateJobErrorMessage(const UpdateErrorType &updateJobErrorMessage);
    void setUpdateFailedInfo(const UpdateErrorType &errorType);

Q_SIGNALS:
    void notifyUpdateState(int);
    void requestUpdates(ClassifyUpdateType type);
    void requestUpdateCtrl(ClassifyUpdateType type, int ctrlType);
    void requestOpenAppStroe();
    void requestFixError(const ClassifyUpdateType &updateType, const QString &error);

public Q_SLOTS:
    void onShowUpdateCtrl();

private Q_SLOTS:
    void onFullUpdateClicked();
    void onRequestRefreshSize();
    void onRequestRefreshWidget();
    void onClassityUpdateJonErrorChanged(const ClassifyUpdateType &type, const UpdateErrorType &errorType);

private:
    void setStatus(const UpdatesStatus &status);

    void setSystemUpdateStatus(const UpdatesStatus &status);
    void setSafeUpdateStatus(const UpdatesStatus &status);
    void setUnkonowUpdateStatus(const UpdatesStatus &status);

    void setSystemUpdateInfo(UpdateItemInfo *updateItemInfo);
    void setSafeUpdateInfo(UpdateItemInfo *updateItemInfo);
    void setUnkonowUpdateInfo(UpdateItemInfo *updateItemInfo);
    void setAllUpdateInfo(QMap<ClassifyUpdateType, UpdateItemInfo *> updateInfoMap);

    void setProgressValue(const double value);
    void setLowBattery(const bool &lowBattery);
    void setUpdateProgress(const double value);
    void setRecoverConfigValid(const bool value);
    void setRecoverRestoring(const bool value);
    void setShowInfo(const UiActiveState value);
    void setActiveState(const UiActiveState &activestate);
    void showUpdateInfo();

    void onChangeUpdatesAvailableStatus();
    void onRecoverBackupFinshed();
    void onRecoverBackupFailed();
    void onUpdateSuccessed();
    void onUpdateFailed();

    void initUpdateItem(UpdateSettingItem *updateItem);
    bool checkUpdateItemIsUpdateing(UpdateSettingItem *updateItem, ClassifyUpdateType type);
    void showAllUpdate();

private:
    UpdateModel *m_model;
    UpdatesStatus m_status;
    LoadingItem *m_checkUpdateItem;
    ResultItem *m_resultItem;
    DownloadProgressBar *m_progress;
    DownloadProgressBar *m_fullProcess;
    DCC_NAMESPACE::SettingsGroup *m_upgradeWarningGroup;
    SummaryItem *m_summary;
    SummaryItem *m_upgradeWarning;
    QLabel *m_powerTip;
    QLabel *m_reminderTip;
    QLabel *m_noNetworkTip;
    QSettings *m_qsettings;
    QString m_systemVersion;

    UiActiveState m_activeState;
    QWidget *m_updateList;
    QLabel *m_authorizationPrompt;
    bool m_isUpdateingAll;

    QPushButton *m_checkUpdateBtn;
    QLabel *m_lastCheckTimeTip;

    QPushButton *m_CheckAgainBtn;
    QLabel *m_lastCheckAgainTimeTip;

    DTK_WIDGET_NAMESPACE::DLabel *m_versionTip;

    DTK_WIDGET_NAMESPACE::DSpinner *m_spinner;
    DTK_WIDGET_NAMESPACE::DLabel *m_updateTipsLab;
    DTK_WIDGET_NAMESPACE::DLabel *m_updateSizeLab;
    DTK_WIDGET_NAMESPACE::DLabel *m_updateingTipsLab;
    QPushButton *m_fullUpdateBtn;

    qlonglong m_updateSize;

    SystemUpdateItem *m_systemUpdateItem;
    SafeUpdateItem *m_safeUpdateItem;
    UnknownUpdateItem *m_unknownUpdateItem;

    QMap<ClassifyUpdateType, UpdateSettingItem *> m_updateingItemMap;
    DCC_NAMESPACE::SettingsGroup *m_updateSummaryGroup;

    UpdateErrorType m_updateJobErrorMessage;
    QMap<UpdateErrorType, Error_Info> m_UpdateErrorInfoMap;
};

