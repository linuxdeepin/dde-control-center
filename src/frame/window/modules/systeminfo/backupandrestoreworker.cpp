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
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "manual_backup" << "--path" << choosePath);
    process->waitForFinished(-1);

    if (process->exitCode() == 5) {
        return ErrorType::SpaceError;
    }

    if (process->exitCode() != 0 && process->exitCode() != 5) {
        return ErrorType::ToolError;
    }

    restart();
    return setGrubAndRestart();
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

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "system_backup" << "--path" << choosePath);
    process->waitForFinished(-1);

    if (process->exitCode() == 5) {
        return ErrorType::SpaceError;
    }

    if (process->exitCode() != 0 && process->exitCode() != 5) {
        qDebug() << process->readAllStandardError();
        return ErrorType::ToolError;
    }

    restart();
    return setGrubAndRestart();
}

ErrorType BackupAndRestoreWorker::doManualRestore()
{
    const QString& selectPath = m_model->restoreDirectory();

    if (selectPath.isEmpty() || !QDir(selectPath).exists()) {
        return ErrorType::PathError2;
    }
    auto checkValid = [](const QString& filePath) -> bool {
        QScopedPointer<QProcess> process(new QProcess);
        process->setProgram("deepin-clone");
        process->setArguments({"--dim-info", filePath});
        process->start();
        process->waitForFinished();

        return process->exitCode() == 0 && process->exitStatus() == QProcess::NormalExit;
    };

    if (!checkValid(QString("%1/system.dim").arg(selectPath))) {
        qDebug() << Q_FUNC_INFO << "md5 check failed!";
        return ErrorType::MD5Error;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "manual_restore" << "--path" << selectPath);
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qDebug() << Q_FUNC_INFO << "restore tool run failed!";
        return ErrorType::ToolError;
    }

    restart();
    return setGrubAndRestart();
}

ErrorType BackupAndRestoreWorker::doSystemRestore()
{
    const bool formatData = m_model->formatData();

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "system_restore" << (formatData ? "--formatData" : ""));
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qDebug() << Q_FUNC_INFO << "restore tool run failed!";
        return ErrorType::ToolError;
    }

    return setGrubAndRestart();
}

ErrorType BackupAndRestoreWorker::setGrubAndRestart()
{
    QScopedPointer<GrubInter> grub(new GrubInter(GrubInter::staticInterfaceName(), "/com/deepin/daemon/Grub2", QDBusConnection::systemBus()));
    if (grub->defaultEntry() != "UOS Backup & Restore") {
        QScopedPointer<QDBusPendingCallWatcher> watcher(new QDBusPendingCallWatcher(grub->SetDefaultEntry("UOS Backup & Restore")));
        watcher->waitForFinished();
        if (watcher->isError()) {
            qDebug() << Q_FUNC_INFO << watcher->error();
            return ErrorType::GrubError;
        }

        do {
            QThread::sleep(1);
        } while (grub->updating());
    }

    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();

    restart();
    return ErrorType::NoError;
}
