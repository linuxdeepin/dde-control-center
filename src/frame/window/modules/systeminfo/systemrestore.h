#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <DButtonBox>

DWIDGET_USE_NAMESPACE

class QPushButton;
class QStackedLayout;
namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualBackup;
class ManualRestore;
class BackupAndRestoreModel;
class SystemRestore : public QWidget
{
    Q_OBJECT
public:
    SystemRestore(BackupAndRestoreModel* model, QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetManualBackupDirectory(const QString& path);
    void requestSetSystemBackupDirectory(const QString& path);
    void requestSystemRestore(bool formatData) const;
    void requestManualRestore(const QString& directory) const;

private:
//    DButtonBox* m_buttonBox;
    ManualBackup* m_backupPage;
    ManualRestore* m_restorePage;
    QStackedLayout* m_stackedLayout;
};
}  // namespace systeminfo
}  // namespace DCC_NAMESPACE
