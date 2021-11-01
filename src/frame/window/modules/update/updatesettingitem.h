//
// Created by swq on 2021/9/7.
//

#ifndef UpdateSettingItem_H
#define UpdateSettingItem_H

#include <QWidget>
#include <QLabel>
#include <DFloatingButton>
#include <DCommandLinkButton>
#include <DLabel>
#include <DLineEdit>
#include <DTextEdit>
#include <DTipLabel>
#include <DShadowLine>

#include "widgets/settingsgroup.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsitem.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/detailinfoitem.h"
#include "updatecontrolpanel.h"
#include "modules/update/updateiteminfo.h"

namespace dcc {
namespace update {

class UpdateSettingItem: public dcc::widgets::SettingsItem
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

Q_SIGNALS:
    void UpdateSuccessed();
    void UpdateFailed();
    void recoveryBackupFailed();
    void recoveryBackupSuccessed();
    void requestRefreshSize();

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
    dcc::widgets::SmallLabel *m_icon;
    UpdatesStatus m_status;
    ClassifyUpdateType m_classifyUpdateType;
    qlonglong m_updateSize;

protected:
    updateControlPanel *m_controlWidget;
    dcc::widgets::SettingsGroup *m_settingsGroup;
};

}
}


#endif //UpdateSettingItem_H
