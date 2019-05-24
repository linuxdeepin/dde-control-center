#ifndef SYNCSTATEWIDGET_H
#define SYNCSTATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <utility>
#include <QPushButton>

#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/tipslabel.h"
#include "syncmodel.h"

namespace dcc {
namespace sync {
class SyncStateIcon;
class SyncStateWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit SyncStateWidget(QWidget *parent = nullptr);
    void setModel(const SyncModel* const model);

Q_SIGNALS:
    void requestEnableSync(bool enable);
    void requestSetModuleState(std::pair<SyncModel::SyncType, bool> state);
    void requestLogout();

private:
    void onStateChanged(const std::pair<qint32, QString> &state);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);
    void onModuleStateChanged(std::pair<SyncModel::SyncType, bool> state);
    void onModuleItemSwitched(const bool checked);
    void onAutoSyncChanged(bool autoSync);

private:
    const SyncModel* m_model;
    SyncStateIcon* m_syncIcon;
    QLabel* m_syncStateLbl;
    QLabel* m_lastSyncTimeLbl;
    widgets::TipsLabel* m_tips;
    widgets::SwitchWidget* m_autoSync;
    widgets::SettingsGroup* m_moduleGrp;
    QPushButton* m_logoutBtn;
    QMap<widgets::SwitchWidget*, SyncModel::SyncType> m_syncModuleMap;
};
}
}

#endif // SYNCSTATEWIDGET_H
