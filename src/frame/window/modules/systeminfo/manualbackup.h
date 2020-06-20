#pragma once

#include "interface/namespace.h"
#include "backupandrestoremodel.h"

#include <QWidget>
#include <dfilechooseredit.h>
#include <DTipLabel>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualBackup : public QWidget {
    Q_OBJECT
public:
    explicit ManualBackup(BackupAndRestoreModel* model, QWidget* parent = nullptr);
    void initUI();

    void setTipsVisible(const bool &visible);

Q_SIGNALS:
    void requestSetManualBackupDirectory(const QString& path);
    void requestSetSystemBackupDirectory(const QString& path);

private Q_SLOTS:
    /**
    * @brief ManualBackup::onChoose 槽函数，接收DFileChooserEdit::dialogClosed信号
    * @param code DFileChooserEdit中文件选择对话框的返回值，即QFileDialog::exec返回值
    * @return void
    */
    void onChoose(const int &code);
    void backup();
    void onManualBackupErrorTypeChanged(ErrorType type);

private:
    enum class ActionType{
        ManualBackup,
        SystemBackup
    };
    ActionType m_actionType;
    BackupAndRestoreModel* m_model;
    DFileChooserEdit* m_directoryChooseWidget;
    DTipLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
    QWidget* m_loadingWidget;
};
}
}
