#include "backupandrestoremodel.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

BackupAndRestoreModel::BackupAndRestoreModel(QObject *parent)
    : QObject(parent)
    , m_backupButtonEnabled(true)
    , m_restoreButtonEnabled(true)
    , m_formatData(false)
    , m_manualBackupErrorType(ErrorType::NoError)
{

}

void BackupAndRestoreModel::setBackupButtonEnabled(bool backupButtonEnabled)
{
    m_backupButtonEnabled = backupButtonEnabled;

    Q_EMIT backupButtonEnabledChanged(backupButtonEnabled);
}

void BackupAndRestoreModel::setRestoreButtonEnabled(bool restoreButtonEnabled)
{
    m_restoreButtonEnabled = restoreButtonEnabled;

    Q_EMIT restoreButtonEnabledChanged(restoreButtonEnabled);
}

void BackupAndRestoreModel::setBackupDirectory(const QString &directory)
{
    m_backupDirectory = directory;
}

void BackupAndRestoreModel::setRestoreDirectory(const QString &directory)
{
    m_restoreDirectory = directory;
}

void BackupAndRestoreModel::setFormatData(bool formatData)
{
    m_formatData = formatData;
}

void BackupAndRestoreModel::setManualRestoreCheckFailed(ErrorType errorType)
{
    if (m_manualBackupErrorType == errorType) {
        return;
    }

    m_manualBackupErrorType = errorType;

    Q_EMIT manualRestoreErrorTypeChanged(errorType);
}
