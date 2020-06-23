#include "backupandrestoreworker.h"
#include "backupandrestoremodel.h"

#include <QSharedPointer>
#include <QProcess>
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
    , m_grubInter(new GrubInter("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2", QDBusConnection::systemBus(), this))
{
    m_grubInter->setSync(false, false);
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
        const bool result = watcher->result();
        qDebug() << Q_FUNC_INFO << result;
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

    if (choosePath.isEmpty() || !choosePath.startsWith("/media")) {
        return ErrorType::PathError;
    }

    //check choosePath file system type
    QStringList fileSystems{"ext4", "ntfs"};
    QStorageInfo storageInfo(choosePath);
    if (!fileSystems.contains(storageInfo.fileSystemType().toLower())) {
        return ErrorType::FsError;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "manual_backup" << "--path" << m_model->backupDirectory());
    process->waitForFinished();

    //choosePath don't have enough space
    if (process->exitCode() == 5) {
        return ErrorType::SpaceError;
    }

    if (process->exitCode() != 0) {
        return ErrorType::ToolError;
    }

    QScopedPointer<QDBusPendingCallWatcher> watcher(new QDBusPendingCallWatcher(m_grubInter->SetDefaultEntry("UOS Backup & Restore")));
    watcher->waitForFinished();
    if (watcher->isError()) {
        qWarning() << Q_FUNC_INFO << watcher->error();
        return ErrorType::GrubError;
    }

    QThread::sleep(5);

    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();

    return ErrorType::NoError;
}

ErrorType BackupAndRestoreWorker::doManualRestore()
{
    const QString& selectPath = m_model->restoreDirectory();

    auto checkValid = [](const QString& filePath) -> bool {
        QScopedPointer<QProcess> process(new QProcess);
        process->setProgram("deepin-clone");
        process->setArguments({"--dim-info", filePath});
        process->start();
        process->waitForFinished();

        return process->exitCode() == 0 && process->exitStatus() == QProcess::NormalExit;
    };

    if (!checkValid(QString("%1/system.dim").arg(selectPath))) {
        qWarning() << Q_FUNC_INFO << "md5 check failed!";
        return ErrorType::MD5Error;
    }

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "manual_restore" << "--path" << selectPath);
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qWarning() << Q_FUNC_INFO << "restore tool run failed!";
        return ErrorType::ToolError;
    }

    QScopedPointer<QDBusPendingCallWatcher> watcher(new QDBusPendingCallWatcher(m_grubInter->SetDefaultEntry("UOS Backup & Restore")));
    watcher->waitForFinished();
    if (watcher->isError()) {
        qWarning() << Q_FUNC_INFO << watcher->error();
        return ErrorType::GrubError;
    }

    QThread::sleep(5);

    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();

    return ErrorType::NoError;
}

ErrorType BackupAndRestoreWorker::doSystemRestore()
{
    const bool formatData = m_model->formatData();

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "system_restore" << (formatData ? "--formatData" : ""));
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qWarning() << Q_FUNC_INFO << "restore tool run failed!";
        return ErrorType::ToolError;
    }

    QScopedPointer<QDBusPendingCallWatcher> watcher(new QDBusPendingCallWatcher(m_grubInter->SetDefaultEntry("UOS Backup & Restore")));
    watcher->waitForFinished();
    if (watcher->isError()) {
        qWarning() << Q_FUNC_INFO << watcher->error();
        return ErrorType::GrubError;
    }

    QThread::sleep(5);

    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();

    return ErrorType::NoError;
}
