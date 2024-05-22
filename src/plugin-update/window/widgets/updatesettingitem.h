//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UpdateSettingItem_H
#define UpdateSettingItem_H

#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/detailinfoitem.h"
#include "widgets/updatecontrolpanel.h"
#include "widgets/updateiteminfo.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

DTK_BEGIN_NAMESPACE
class DFloatingButton;
class DCommandLinkButton;
class DLabel;
class DLineEdit;
class DTextEdit;
class DTipLabel;
class DShadowLine;
DTK_END_NAMESPACE

struct Error_Info {
    UpdateErrorType ErrorType;
    QString errorMessage;
    QString errorTips;
};

class UpdateSettingItem: public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit UpdateSettingItem(QWidget *parent = nullptr);
    void initUi();
    void initConnect();

    void setIconVisible(bool show);
    void setIcon(QString path);
    void setProgress(double value);

    ButtonStatus getCtrlButtonStatus();

    virtual void setData(UpdateItemInfo *updateItemInfo);

    UpdatesStatus status() const;
    void setStatus(const UpdatesStatus &status);

    ClassifyUpdateType classifyUpdateType() const;
    void setClassifyUpdateType(const ClassifyUpdateType &classifyUpdateType);

    qlonglong updateSize() const;
    void setUpdateSize(const qlonglong &updateSize);

    double getProgressVlaue() const;
    void setProgressVlaue(double progressVlaue);

    UpdateErrorType getUpdateJobErrorMessage() const;
    void setUpdateJobErrorMessage(const UpdateErrorType &updateJobErrorMessage);

    void setUpdateFailedInfo();

Q_SIGNALS:
    void UpdateSuccessed();
    void UpdateFailed();
    void recoveryBackupFailed();
    void recoveryBackupSuccessed();
    void requestRefreshSize();
    void requestRefreshWidget();
    void requestFixError(const ClassifyUpdateType &updateType, const QString &error);

    void requestUpdate(ClassifyUpdateType type);
    void requestUpdateCtrl(ClassifyUpdateType type, int ctrlType);

public Q_SLOTS:
    virtual void showMore();

    void onStartUpdate();
    void onStartDownload();
    void onPauseDownload();
    void onRetryUpdate();

    void onUpdateStatuChanged(const UpdatesStatus &status);
    void onUpdateProgressChanged(const double &value);

private:
    QLabel *m_icon;
    UpdatesStatus m_status;
    ClassifyUpdateType m_classifyUpdateType;
    qlonglong m_updateSize;
    double m_progressVlaue;
    UpdateErrorType m_updateJobErrorMessage;
    QMap<UpdateErrorType, Error_Info> m_UpdateErrorInfoMap;

protected:
    updateControlPanel *m_controlWidget;
    DCC_NAMESPACE::SettingsGroup *m_settingsGroup;
};

#endif //UpdateSettingItem_H
