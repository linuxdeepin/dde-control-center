#ifndef BACKUPANDRESTOREMODEL_H
#define BACKUPANDRESTOREMODEL_H

#include "interface/namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {
namespace systeminfo {
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

    inline bool manualRestoreCheckFailed() const {
        return m_manualRestoreCheckFailed;
    }

Q_SIGNALS:
    void backupButtonEnabledChanged(bool backupButtonEnabled) const;
    void restoreButtonEnabledChanged(bool restoreButtonEnabled) const;
    void manualRestoreCheckFailedChanged(bool manualRestoreCheckFailed) const;

public Q_SLOTS:
    void setBackupButtonEnabled(bool backupButtonEnabled);
    void setRestoreButtonEnabled(bool restoreButtonEnabled);
    void setBackupDirectory(const QString& directory);
    void setRestoreDirectory(const QString& directory);
    void setFormatData(bool formatData);
    void setManualRestoreCheckFailed(bool manualRestoreCheckFailed);

private:
    bool m_backupButtonEnabled;
    bool m_restoreButtonEnabled;
    bool m_formatData;
    bool m_manualRestoreCheckFailed;
    QString m_backupDirectory;
    QString m_restoreDirectory;
};
}
}

#endif // BACKUPANDRESTOREMODEL_H
