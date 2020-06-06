#pragma once

#include "interface/namespace.h"
#include "backupandrestoremodel.h"

#include <QWidget>
#include <dfilechooseredit.h>
#include <DWaterProgress>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualBackup : public QWidget {
    Q_OBJECT
public:
    explicit ManualBackup(BackupAndRestoreModel* model, QWidget* parent = nullptr);

Q_SIGNALS:
    void requestSetManualBackupDirectory(const QString& path);
    void requestSetSystemBackupDirectory(const QString& path);

private Q_SLOTS:
    void onChoose();
    void backup();
    void onManualBackupErrorTypeChanged(ErrorType type);

private:
    enum class ActionType{
        ManualBackup,
        SystemBackup
    };
    ActionType m_actionType;
    BackupAndRestoreModel* m_model;
    DFileChooserEdit* m_directoryChooseWidget;
    QLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
    DWaterProgress* m_loadingIndicator;
};
}
}
