#ifndef SYNCSTATEWIDGET_H
#define SYNCSTATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <utility>

#include "widgets/settingsitem.h"

namespace dcc {
namespace sync {
class SyncModel;
class SyncStateWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit SyncStateWidget(QWidget *parent = nullptr);
    void setModel(const SyncModel* const model);

private:
    void onStateChanged(const std::pair<qint32, QString> &state);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);

private:
    const SyncModel* m_model;
    QLabel* m_syncIcon;
    QLabel* m_syncState;
    QLabel* m_lastSyncTime;
};
}
}

#endif // SYNCSTATEWIDGET_H
