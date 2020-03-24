#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>
#include <com_deepin_daemon_grub2.h>
#include <DWaterProgress>

DWIDGET_USE_NAMESPACE

using GrubInter = com::deepin::daemon::Grub2;

class QLabel;
class QPushButton;
class RestoreItem;
class QCheckBox;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
namespace systeminfo {
class BackupAndRestoreModel;
class ManualRestore : public QWidget
{
    Q_OBJECT
public:
    explicit ManualRestore(BackupAndRestoreModel* model, QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSystemRestore(bool formatData) const;
    void requestManualRestore(const QString& directory) const;

private Q_SLOTS:
    void onItemChecked();
    void restore();
    void onManualRestoreCheckFailed(bool failed);

private:
    enum class ActionType {
        RestoreSystem,
        ManualRestore
    };

private:
    BackupAndRestoreModel* m_model;
    QCheckBox* m_saveUserDataCheckBox;
    DFileChooserEdit* m_directoryChooseWidget;
    QLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
    RestoreItem* m_systemRestore;
    RestoreItem* m_manualRestore;
    ActionType m_actionType;
    DWaterProgress* m_loadingIndicator;
};
}
}
