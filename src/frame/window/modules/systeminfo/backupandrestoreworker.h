#ifndef BACKUPANDRESTOREWORKER_H
#define BACKUPANDRESTOREWORKER_H

#include "interface/namespace.h"
#include "backupandrestoremodel.h"

#include <QObject>

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
    /**
     * @brief doWorkTool 运行deepin-recovery-tool
     * @param args 工具所需的参数列表
     * @return 错误值，0表示无错误
     */
    ErrorType doWorkTool(const QStringList &args);

private:
    BackupAndRestoreModel* m_model;
};
}
}

#endif // BACKUPANDRESTOREWORKER_H
