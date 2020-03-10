#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>
#include <com_deepin_daemon_grub2.h>

DWIDGET_USE_NAMESPACE

using GrubInter = com::deepin::daemon::Grub2;

class QLabel;
class QPushButton;
class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {
    namespace systeminfo {
        class ManualBackup : public QWidget {
            Q_OBJECT
        public:
            explicit ManualBackup(QWidget* parent = nullptr);

        private Q_SLOTS:
            void onChoose();
            void backup();
            void onGrubSetFinished(QDBusPendingCallWatcher *self);

        private:
            DFileChooserEdit* m_directoryChooseWidget;
            QLabel          * m_tipsLabel;
            QPushButton* m_backupBtn;
            GrubInter* m_grubInter;
        };
    }
}
