#ifndef SYNCWIDGET_H
#define SYNCWIDGET_H

#include "modules/modulewidget.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace dcc {
namespace widgets {
class SettingsGroup;
class NextPageWidget;
}
namespace cloudsync {
class SyncModel;
class SyncWidget : public ModuleWidget {
    Q_OBJECT
public:
    explicit SyncWidget();
    void setModel(const SyncModel* const model);

Q_SIGNALS:
    void requestLogin() const;
    void requestShowSyncDetails();

private:
    void onUserInfoChanged(const QVariantMap& userinfo);

private:
    QPushButton *m_accountBtn;
    const SyncModel* m_model;
    widgets::SettingsGroup* m_userButtonGrp;
    widgets::NextPageWidget* m_userButton;
};
}  // namespace cloudsync
}  // namespace dcc

#endif  // SYNCWIDGET_H
