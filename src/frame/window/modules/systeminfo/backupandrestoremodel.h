#ifndef BACKUPANDRESTOREMODEL_H
#define BACKUPANDRESTOREMODEL_H

#include "interface/namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {
namespace systeminfo {
enum ErrorType {
    NoError,
    ToolError,
    MD5Error,
    MissingBoot,
    MissingRoot,
    GrubError,
    PathError,
    PathError2,
    SpaceError,
    FsError
};

class BackupAndRestoreModel : public QObject
{
    Q_OBJECT
public:
    explicit BackupAndRestoreModel(QObject *parent = nullptr);

    inline bool backupButtonEnabled() const {
        return m_backupButtonEnabled;
    }

    inline bool restoreButtonEnabled() const {
        return m_restoreButtonEnabled;
    }

    inline bool formatData() const {
        return m_formatData;
    }

    inline QString backupDirectory() const {
        return m_backupDirectory;
    }

    inline QString restoreDirectory() const {
        return m_restoreDirectory;
    }

    inline ErrorType manualRestoreErrorType() const {
        return m_manualBackupErrorType;
    }

    inline ErrorType manualBackupErrorType() const {
        return m_manualBackupErrorType;
    }

Q_SIGNALS:
    void backupButtonEnabledChanged(bool backupButtonEnabled) const;
    void restoreButtonEnabledChanged(bool restoreButtonEnabled) const;
    void manualRestoreErrorTypeChanged(ErrorType type) const;
    void manualBackupErrorTypeChanged(ErrorType type) const;

public Q_SLOTS:
    void setBackupButtonEnabled(bool backupButtonEnabled);
    void setRestoreButtonEnabled(bool restoreButtonEnabled);
    void setBackupDirectory(const QString& directory);
    void setRestoreDirectory(const QString& directory);
    void setFormatData(bool formatData);
    void setManualRestoreErrorType(ErrorType errorType);
    void setManualBackupErrorType(ErrorType type);

private:
    bool m_backupButtonEnabled;
    bool m_restoreButtonEnabled;
    bool m_formatData;
    ErrorType m_manualBackupErrorType;
    ErrorType m_manualRestoreErrorType;
    QString m_backupDirectory;
    QString m_restoreDirectory;
};
}
}

#endif // BACKUPANDRESTOREMODEL_H
