#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
namespace systeminfo {
class BackupAndRestoreModel;
class ManualBackup : public QWidget {
    Q_OBJECT
public:
    explicit ManualBackup(BackupAndRestoreModel* model, QWidget* parent = nullptr);

Q_SIGNALS:
    void requestSetBackupDirectory(const QString& path);

private Q_SLOTS:
    void onChoose();
    void backup();

private:
    BackupAndRestoreModel* m_model;
    DFileChooserEdit* m_directoryChooseWidget;
    QLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
};
}
}
