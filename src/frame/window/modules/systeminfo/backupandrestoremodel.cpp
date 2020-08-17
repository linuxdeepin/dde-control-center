#include "backupandrestoremodel.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

BackupAndRestoreModel::BackupAndRestoreModel(QObject *parent)
    : QObject(parent)
    , m_backupButtonEnabled(true)
    , m_restoreButtonEnabled(true)
    , m_formatData(false)
    , m_manualBackupErrorType(ErrorType::NoError)
    , m_manualRestoreErrorType(ErrorType::NoError)
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

void BackupAndRestoreModel::setFormatData(bool formatData)
{
    m_formatData = formatData;
}

void BackupAndRestoreModel::setManualRestoreErrorType(ErrorType errorType)
{
    m_manualRestoreErrorType = errorType;

    Q_EMIT manualRestoreErrorTypeChanged(errorType);
}

void BackupAndRestoreModel::setManualBackupErrorType(ErrorType type)
{
    m_manualBackupErrorType = type;

    Q_EMIT manualBackupErrorTypeChanged(type);
}
