#ifndef BACKUPANDRESTOREWORKER_H
#define BACKUPANDRESTOREWORKER_H

#include "interface/namespace.h"
#include "backupandrestoremodel.h"

#include <QObject>
#include <com_deepin_daemon_grub2.h>

using GrubInter = com::deepin::daemon::Grub2;

namespace DCC_NAMESPACE {
namespace systeminfo {
class BackupAndRestoreWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackupAndRestoreWorker(BackupAndRestoreModel* model, QObject *parent = nullptr);

    void restart();

public Q_SLOTS:
    void manualBackup(const QString& directory);
    void systemBackup(const QString& directory);
    void manualRestore(const QString& directory);
    void systemRestore(bool formatData);

private:
    ErrorType doManualBackup();
    ErrorType doSystemBackup();
    ErrorType doManualRestore();
    ErrorType doSystemRestore();

private:
    BackupAndRestoreModel* m_model;
    GrubInter* m_grubInter;
};
}
}

#endif // BACKUPANDRESTOREWORKER_H
