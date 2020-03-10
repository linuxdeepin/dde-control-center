#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>
#include <com_deepin_daemon_grub2.h>

DWIDGET_USE_NAMESPACE

using GrubInter = com::deepin::daemon::Grub2;

class QLabel;
class QPushButton;
class RestoreItem;
class QCheckBox;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualRestore : public QWidget
{
    Q_OBJECT
public:
    explicit ManualRestore(QWidget *parent = nullptr);

private Q_SLOTS:
    void onItemChecked();
    void restore();
    void restoreSystem();
    void restoreManual();
    void onGrubSetFinished(QDBusPendingCallWatcher *self);

private:
    enum class ActionType {
        RestoreSystem,
        ManualRestore
    };

private:
    QCheckBox* m_saveUserDataCheckBox;
    DFileChooserEdit* m_directoryChooseWidget;
    QLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
    RestoreItem* m_systemRestore;
    RestoreItem* m_manualRestore;
    ActionType m_actionType;
    GrubInter* m_grubInter;
};
}
}
