// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/update/common.h"
#include "widgets/utils.h"
#include "updatesettingitem.h"
#include "systemupdateitem.h"
#include "appstoreupdateitem.h"
#include "safeupdateitem.h"
#include "unknownupdateitem.h"
#include "modules/update/updateiteminfo.h"

#include <QWidget>
#include <DSpinner>

class AppUpdateInfo;
class QPushButton;

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateModel;
class DownloadInfo;
class SummaryItem;
class DownloadProgressBar;
class ResultItem;
class SystemUpdateItem;
class AppstoreUpdateItem;
class SafeUpdateItem;
class UnknownUpdateItem;
class UpdateSettingItem;
}

namespace widgets {
class SettingsGroup;
class TipsLabel;
}
}

using namespace dcc::update;

namespace DCC_NAMESPACE {
namespace update {

class LoadingItem;

class UpdateCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateCtrlWidget(UpdateModel *model, QWidget *parent = 0);
    ~UpdateCtrlWidget();

    void initConnect();
    void setModel(UpdateModel *model);
    void setSystemVersion(const QString &version);

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
    void setRecoverBackingUp(const bool value);
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
    bool continueUpdate();

private:
    UpdateModel *m_model;
    UpdatesStatus m_status;
    LoadingItem *m_checkUpdateItem;
    ResultItem *m_resultItem;
    DownloadProgressBar *m_progress;
    DownloadProgressBar *m_fullProcess;
    dcc::widgets::SettingsGroup *m_upgradeWarningGroup;
    SummaryItem *m_summary;
    SummaryItem *m_upgradeWarning;
    dcc::widgets::TipsLabel *m_powerTip;
    dcc::widgets::TipsLabel *m_reminderTip;
    dcc::widgets::TipsLabel *m_noNetworkTip;
    QSettings *m_qsettings;
    QString m_systemVersion;
    bool m_bRecoverBackingUp;
    bool m_bRecoverConfigValid;
    bool m_bRecoverRestoring;
    UiActiveState m_activeState;
    dcc::ContentWidget *m_updateList;
    dcc::widgets::TipsLabel *m_authorizationPrompt;
    bool m_isUpdateingAll;

    QPushButton *m_checkUpdateBtn;
    dcc::widgets::TipsLabel *m_lastCheckTimeTip;

    QPushButton *m_CheckAgainBtn;
    dcc::widgets::TipsLabel *m_lastCheckAgainTimeTip;

    DLabel *m_versrionTip;

    DTK_WIDGET_NAMESPACE::DSpinner *m_spinner;
    DLabel *m_updateTipsLab;
    DLabel *m_updateSizeLab;
    DLabel *m_updateingTipsLab;
    QPushButton *m_fullUpdateBtn;

    qlonglong m_updateSize;

    SystemUpdateItem *m_systemUpdateItem;
    SafeUpdateItem *m_safeUpdateItem;
    UnknownUpdateItem *m_unknownUpdateItem;

    QMap<ClassifyUpdateType, UpdateSettingItem *> m_updateingItemMap;
    dcc::widgets::SettingsGroup *m_updateSummaryGroup;

    UpdateErrorType m_updateJobErrorMessage;
    QMap<UpdateErrorType, Error_Info> m_UpdateErrorInfoMap;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
