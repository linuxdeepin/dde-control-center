#include "backupandrestoreworker.h"
#include "backupandrestoremodel.h"

#include <QSharedPointer>
#include <QProcess>
#include <QTextStream>
#include <DDBusSender>

#include <QFuture>
#include <QtConcurrent>
#include <DDialog>
#include <QScopedPointer>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

/**
 * @brief deepin-recovery-tool工具返回的错误值
 */
enum ToolErrorType {
    NoError = 0,
    SpaceError,     //磁盘空间不足
    FsError,        //文件系统不支持
    PathError,      //不是有效路径
    LocationError,  //路径位置不允许
    MD5Error        //备份文件错误
};

BackupAndRestoreWorker::BackupAndRestoreWorker(BackupAndRestoreModel* model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{

}

void BackupAndRestoreWorker::restart()
{
    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();
}

void BackupAndRestoreWorker::manualBackup(const QString &directory)
{
    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher] {
        const ErrorType type = watcher->result();
        qDebug() << Q_FUNC_INFO << "result type: " << type;
        m_model->setManualBackupErrorType(type);
        m_model->setBackupButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doWorkTool,
        QStringList() << "/bin/deepin-recovery-tool" << "-a" << "manual_backup" << "-p" << directory);
    watcher->setFuture(future);

    m_model->setBackupButtonEnabled(false);
}

void BackupAndRestoreWorker::systemBackup(const QString &directory)
{
    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher] {
        const ErrorType type = watcher->result();
        qDebug() << Q_FUNC_INFO << "result type: " << type;
        m_model->setManualBackupErrorType(type);
        m_model->setBackupButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doWorkTool,
        QStringList() << "/bin/deepin-recovery-tool" << "-a" << "system_backup" << "-p" << directory);
    watcher->setFuture(future);

    m_model->setBackupButtonEnabled(false);
}

void BackupAndRestoreWorker::manualRestore(const QString &directory)
{
    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<ErrorType>::finished, [this, watcher] {
        const ErrorType result = watcher->result();
        qDebug() << Q_FUNC_INFO << result;
        m_model->setManualRestoreErrorType(result);
        m_model->setRestoreButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doWorkTool,
        QStringList() << "/bin/deepin-recovery-tool" << "-a" << "manual_restore" << "-p" << directory);
    watcher->setFuture(future);

    m_model->setRestoreButtonEnabled(false);
}

void BackupAndRestoreWorker::systemRestore(bool formatData)
{
    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<ErrorType>::finished, [this, watcher] {
        const ErrorType result = watcher->result();
        qDebug() << Q_FUNC_INFO << result;
        m_model->setManualRestoreErrorType(result);
        m_model->setRestoreButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doWorkTool,
        QStringList() << "/bin/deepin-recovery-tool" << "-a" << "system_restore" << (formatData ? "-f" : ""));
    watcher->setFuture(future);

    m_model->setRestoreButtonEnabled(false);
}

ErrorType BackupAndRestoreWorker::doWorkTool(const QStringList &args)
{
    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", args);
    process->waitForFinished();

    const int &exitCode = process->exitCode();
    qDebug() << "exit code:" << exitCode;
    switch (exitCode)
    {
    case ToolErrorType::NoError:
        restart();
        return ErrorType::NoError;
    case ToolErrorType::SpaceError:
        return ErrorType::SpaceError;
    case ToolErrorType::FsError:
        return ErrorType::FsError;
    case ToolErrorType::PathError:
        return ErrorType::PathError;
    case ToolErrorType::LocationError:
        return ErrorType::LocationError;
    case ToolErrorType::MD5Error:
        return ErrorType::MD5Error;
    default:
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }
}