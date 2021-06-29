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
#include <QStorageInfo>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

/**
 * @brief deepin-recovery-tool工具返回的错误值
 */
enum ToolErrorType {
    NoError = 0,
    ToolError,
    GrubError,
    SpaceError
};

//deepin-recovery-tool超时时间设置为1分钟
const int TimeOut = 60000;

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
    m_model->setBackupDirectory(directory);

    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher] {
        const ErrorType type = watcher->result();
        qDebug() << Q_FUNC_INFO << "result type: " << type;
        m_model->setManualBackupErrorType(type);
        m_model->setBackupButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doManualBackup);
    watcher->setFuture(future);

    m_model->setBackupButtonEnabled(false);
}

void BackupAndRestoreWorker::systemBackup(const QString &directory)
{
    m_model->setBackupDirectory(directory);

    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher] {
        const ErrorType type = watcher->result();
        qDebug() << Q_FUNC_INFO << "result type: " << type;
        m_model->setManualBackupErrorType(type);
        m_model->setBackupButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doSystemBackup);
    watcher->setFuture(future);

    m_model->setBackupButtonEnabled(false);
}

void BackupAndRestoreWorker::manualRestore(const QString &directory)
{
    m_model->setRestoreDirectory(directory);

    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<ErrorType>::finished, [this, watcher] {
        const ErrorType result = watcher->result();
        qDebug() << Q_FUNC_INFO << result;
        m_model->setManualRestoreErrorType(result);
        m_model->setRestoreButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doManualRestore);
    watcher->setFuture(future);

    m_model->setRestoreButtonEnabled(false);
}

void BackupAndRestoreWorker::systemRestore(bool formatData)
{
    m_model->setFormatData(formatData);

    QFutureWatcher<ErrorType> *watcher = new QFutureWatcher<ErrorType>(this);
    connect(watcher, &QFutureWatcher<ErrorType>::finished, [this, watcher] {
        const ErrorType result = watcher->result();
        qDebug() << Q_FUNC_INFO << result;
        m_model->setManualRestoreErrorType(result);
        m_model->setRestoreButtonEnabled(true);
        watcher->deleteLater();
    });

    QFuture<ErrorType> future = QtConcurrent::run(this, &BackupAndRestoreWorker::doSystemRestore);
    watcher->setFuture(future);

    m_model->setRestoreButtonEnabled(false);
}

ErrorType BackupAndRestoreWorker::doManualBackup()
{
    const QString& choosePath { m_model->backupDirectory()};

    if (choosePath.isEmpty() || !QDir(choosePath).exists()) {
        return ErrorType::PathError2;
    }
    if (!choosePath.startsWith("/media")) {
        return ErrorType::PathError;
    }

    //check choosePath file system type
    QStringList fileSystems{"ext4", "ntfs-3g", "fuseblk"};
    QStorageInfo storageInfo(choosePath);
    if (!fileSystems.contains(storageInfo.fileSystemType().toLower())) {
        return ErrorType::FsError;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/deepin-recovery-tool" << "--actionType" << "manual_backup" << "--path" << choosePath);
    process->waitForFinished(TimeOut);

    const int &exitCode = process->exitCode();
    if (exitCode == ToolErrorType::SpaceError) {
        return ErrorType::SpaceError;
    }

    if (exitCode == ToolErrorType::GrubError) {
        return ErrorType::GrubError;
    }

    if (exitCode != ToolErrorType::NoError) {
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }

    restart();
    return ErrorType::NoError;
}

ErrorType BackupAndRestoreWorker::doSystemBackup()
{
    auto checkMountPoint = [](const QString &path)->QString const{
        if (path.isEmpty()) return "Error";
        QScopedPointer<QProcess> process(new QProcess);
        process->start("df", {path});
        process->waitForFinished(-1);
        QTextStream stream(process->readAllStandardOutput());
        QString line;
        while (stream.readLineInto(&line)) {
            line = line.simplified();
            if (line.startsWith("/dev")) {
                return line.split(' ').last();
            }
        }
        return "Error";
    };
    const QString& choosePath { m_model->backupDirectory()};

    if (choosePath.isEmpty() || !QDir(choosePath).exists()) {
        return ErrorType::PathError2;
    }

    if (checkMountPoint(choosePath) == "Error" || checkMountPoint(choosePath) == "/" || checkMountPoint(choosePath) == "/boot") {
        return ErrorType::PathError;
    }

    //check choosePath file system type
    QStringList fileSystems{"ext4", "ntfs-3g", "fuseblk"};
    QStorageInfo storageInfo(choosePath);
    if (!fileSystems.contains(storageInfo.fileSystemType().toLower())) {
        return ErrorType::FsError;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/deepin-recovery-tool" << "--actionType" << "system_backup" << "--path" << choosePath);
    process->waitForFinished(TimeOut);

    const int &exitCode = process->exitCode();
    if (exitCode == ToolErrorType::SpaceError) {
        return ErrorType::SpaceError;
    }

    if (exitCode == ToolErrorType::GrubError) {
        return ErrorType::GrubError;
    }

    if (exitCode != ToolErrorType::NoError) {
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }

    restart();
    return ErrorType::NoError;
}

ErrorType BackupAndRestoreWorker::doManualRestore()
{
    const QString& selectPath = m_model->restoreDirectory();

    if (selectPath.isEmpty() || !QDir(selectPath).exists()) {
        return ErrorType::PathError2;
    }
    const QString &recoveryPath{ "/etc/deepin/system-recovery.conf" };
    QSettings settings(recoveryPath, QSettings::IniFormat);
    const bool isFullDiskEncryption{ settings.value("FDE").toBool() };

    auto checkValid = [](const QString& filePath) -> bool {
        QScopedPointer<QProcess> process(new QProcess);
        process->setProgram("deepin-clone");
        process->setArguments({"--dim-info", filePath});
        process->start();
        process->waitForFinished();

        return process->exitCode() == 0 && process->exitStatus() == QProcess::NormalExit;
    };

    QString file;
    isFullDiskEncryption ? file = "boot" : file = "system";

    if (!checkValid(QString("%1/%2.dim").arg(selectPath).arg(file))) {
        qDebug() << Q_FUNC_INFO << "md5 check failed!";
        return ErrorType::MD5Error;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/deepin-recovery-tool" << "--actionType" << "manual_restore" << "--path" << selectPath);
    process->waitForFinished(TimeOut);

    const int &exitCode = process->exitCode();
    if (exitCode == ToolErrorType::GrubError) {
        return ErrorType::GrubError;
    }

    if (exitCode != ToolErrorType::NoError) {
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }

    restart();
    return ErrorType::NoError;
}

ErrorType BackupAndRestoreWorker::doSystemRestore()
{
    const bool formatData = m_model->formatData();

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/deepin-recovery-tool" << "--actionType" << "system_restore" << (formatData ? "--formatData" : ""));
    process->waitForFinished(TimeOut);

    const int &exitCode = process->exitCode();
    if (exitCode == ToolErrorType::GrubError) {
        return ErrorType::GrubError;
    }

    if (exitCode != ToolErrorType::NoError) {
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }

    restart();
    return ErrorType::NoError;
}
