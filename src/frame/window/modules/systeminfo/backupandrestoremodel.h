#ifndef BACKUPANDRESTOREMODEL_H
#define BACKUPANDRESTOREMODEL_H

#include "interface/namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {
namespace systeminfo {
enum ErrorType {
    NoError = 0,
    ToolError,
    SpaceError,     //磁盘空间不足
    FsError,        //文件系统不支持
    PathError,      //不是有效路径
    LocationError,  //路径位置不允许
    MD5Error,       //备份文件错误
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
    void setFormatData(bool formatData);
    void setManualRestoreErrorType(ErrorType errorType);
    void setManualBackupErrorType(ErrorType type);

private:
    bool m_backupButtonEnabled;
    bool m_restoreButtonEnabled;
    bool m_formatData;
    ErrorType m_manualBackupErrorType;
    ErrorType m_manualRestoreErrorType;
};
}
}

#endif // BACKUPANDRESTOREMODEL_H
